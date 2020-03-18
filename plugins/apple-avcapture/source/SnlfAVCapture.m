#include "SnlfModule.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <Metal/MTLDevice.h>

#import "SnlfAVDevicesObserver.h"

#include <compositor/vector/simd_base.h>

@interface AVCaptureOutputDelegate : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>

- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;

@end

extern inline NSString *SnlfGetAsPlatformString(const char *utf8String) {
  NSString *platformString = [[NSString alloc] initWithBytesNoCopy:(void *)utf8String length:strlen(utf8String) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  return platformString;
}

struct SnlfAVCaptureContext {
  AVCaptureSession *session;
  AVCaptureOutputDelegate *outputDelegate;
  AVCaptureVideoDataOutput *output;
  dispatch_queue_t queue;
  AVCaptureDevice *device;
  AVCaptureDeviceInput *deviceInput;
  CVMetalTextureCacheRef textureCache;
};

@implementation AVCaptureOutputDelegate {
  struct SnlfAVCaptureContext *context;
}

- (void)captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection {
  CMItemCount count = CMSampleBufferGetNumSamples(sampleBuffer);
  if (count < 1) {
    return;
  }
  
  CMTime timestamp = CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
  CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
  
  CGSize encodedSize = CVImageBufferGetEncodedSize(imageBuffer);
  
  CVMetalTextureRef metalTexture;
  CVMetalTextureCacheCreateTextureFromImage(kCFAllocatorDefault, context->textureCache, imageBuffer, nil, MTLPixelFormatRGBA8Unorm, (size_t)encodedSize.width, (size_t)encodedSize.height, 0, &metalTexture);
  CFRelease(metalTexture);
}

@end

static intptr_t SnlfAVCaptureInit(const SnlfInputDescriptor *descriptor) {
  struct SnlfAVCaptureContext *context = (struct SnlfAVCaptureContext *)malloc(sizeof(struct SnlfAVCaptureContext));
  if (!context) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  AVCaptureSession *session = [[AVCaptureSession alloc] init];
  if (!session) {
    // TODO: Log
    free(context);
    return NULL;
  }
  
  // Set up output
  AVCaptureVideoDataOutput *output = [[AVCaptureVideoDataOutput alloc] init];
  if (!output) {
    // TODO: Log
    [session release];
    free(context);
    return NULL;
  }
  
  AVCaptureOutputDelegate *outputDelegate = [[AVCaptureOutputDelegate alloc] init];
  if (!outputDelegate) {
    // TODO: Log
    [output release];
    [session release];
    free(context);
    return NULL;
  }
  
  dispatch_queue_t queue = dispatch_queue_create(descriptor->identifier, NULL);
  [session addOutput:output];
  [output setSampleBufferDelegate:outputDelegate queue:queue];
  
  // Set up input
  NSString *nsUniqueId = SnlfGetAsPlatformString(descriptor->identifier);
  AVCaptureDevice *device = [AVCaptureDevice deviceWithUniqueID:nsUniqueId];
  [nsUniqueId release];
  if (!device) {
    // TODO: Log
    dispatch_release(queue);
    [outputDelegate release];
    [output release];
    [session release];
    free(context);
    return NULL;
  }
  
  context->session = session;
  context->output = output;
  context->outputDelegate = outputDelegate;
  context->queue = queue;
  context->device = device;
  context->deviceInput = nil;
  return (intptr_t)context;
}

static void SnlfAVCaptureUninit(intptr_t _context) {
  struct SnlfAVCaptureContext *context = (struct SnlfAVCaptureContext *)_context;
  [context->deviceInput release];
  [context->device release];
  dispatch_release(context->queue);
  [context->outputDelegate release];
  [context->output release];
  [context->session release];
  free(context);
}

static void SnlfAVCaptureActivating(intptr_t _context, const CpsrDevice *device) {
  struct SnlfAVCaptureContext *context = (struct SnlfAVCaptureContext *)_context;
  
  id<MTLDevice> mtlDevice = CpsrDeviceGetNativeHandle(device);
  CVMetalTextureCacheRef textureCache;
  CVReturn ret = CVMetalTextureCacheCreate(kCFAllocatorDefault, nil, mtlDevice, nil, &textureCache);
  if (ret != kCVReturnSuccess) {
    // TODO: Log
    return;
  }
  
  NSError *error;
  AVCaptureDeviceInput *deviceInput = [AVCaptureDeviceInput deviceInputWithDevice:context->device error:&error];
  if (!deviceInput) {
    // TODO: Log
    return;
  }
  
  if (![context->session canAddInput:deviceInput]) {
    // TODO: Log
    [deviceInput release];
    return;
  }
  
  [context->session addInput:deviceInput];
  
  // Set to context
  context->deviceInput = deviceInput;
  context->textureCache = textureCache;
  
  [context->session startRunning];
}

static void SnlfAVCaptureDeactivated(intptr_t _context) {
  struct SnlfAVCaptureContext *context = (struct SnlfAVCaptureContext *)_context;
  [context->session stopRunning];
  
  AVCaptureDeviceInput *deviceInput = context->deviceInput;
  if (deviceInput) {
    [context->session removeInput:deviceInput];
    context->deviceInput = nil;
  }
  
  CVMetalTextureCacheRef textureCache = context->textureCache;
  if (textureCache) {
    CFRelease(textureCache);
    context->textureCache = nil;
  }
}

void SnlfModuleGetInfo(SnlfModuleDescriptor *descriptor) {
  strncpy((char *)descriptor->identifier, "SNLF_APPLE_AVCAPTURE", 64);
  strncpy((char *)descriptor->version,    "0.9.0", 24);
  descriptor->comparableVersion = 1;
  strncpy((char *)descriptor->authorName, "mntone", 32);
}

bool SnlfModuleCanLoad(const SnlfLibraryDescriptor *libraryDescriptor) {
  return libraryDescriptor->sevenleafAPIVersion == SNLF_API_VERSION
    && libraryDescriptor->compositorDriverType == CPSR_DRIVER_METAL2;
}

struct SnlfAVCaptureModuleContext {
  SnlfCoreRef core;
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
  union {
    SnlfAVDevicesObserver *observer;
    struct {
      id<NSObject> connectHandler;
      id<NSObject> disconnectHandler;
    };
  };
#endif
};

extern inline void SnlfAVCaptureRegisterDevice(const struct SnlfAVCaptureModuleContext *context, AVCaptureDevice *device) {
  SnlfInputDescriptor inputDesc = {
    .type             = SNLF_INPUT_CAMERA,
    .graphics         = true,
    .sound            = false,
    .push             = false,
    .unique           = true,
    .init             = SnlfAVCaptureInit,
    .uninit           = SnlfAVCaptureUninit,
    .activating       = SnlfAVCaptureActivating,
    .deactivated      = SnlfAVCaptureDeactivated,
  };
  strncpy((char *)inputDesc.identifier, device.uniqueID.UTF8String, SNLF_INPUT_IDENTIFIER_LENGTH);
  strncpy((char *)inputDesc.friendlyName, device.localizedName.UTF8String, SNLF_INPUT_FRIENDLY_NAME_LENGTH);
  SnlfInputRegister(context->core, &inputDesc);
}

extern inline void SnlfAVCaptureRegisterDevices(const struct SnlfAVCaptureModuleContext *context, NSArray<AVCaptureDevice *> *devices) {
  for (NSUInteger i = 0; i < devices.count; ++i) {
    SnlfAVCaptureRegisterDevice(context, devices[i]);
  }
}

intptr_t SnlfModuleLoad(SnlfCoreRef core) {
  struct SnlfAVCaptureModuleContext *context = (struct SnlfAVCaptureModuleContext *)malloc(sizeof(struct SnlfAVCaptureModuleContext));
  if (!context) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  context->core = core;
  
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
  //TODO: macOS 10.15, iOS 10.0 AVCaptureDeviceTypeBuiltInMicrophone
  if (@available(macOS 10.15, *)) {
    NSArray<AVCaptureDeviceType> *deviceTypes = @[AVCaptureDeviceTypeExternalUnknown, AVCaptureDeviceTypeBuiltInWideAngleCamera];
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:deviceTypes mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];

    NSArray<AVCaptureDevice *> *devices = discoverySession.devices;
    SnlfAVCaptureRegisterDevices(context, devices);

    context->observer = [[SnlfAVDevicesObserver alloc] initWithSession:discoverySession context:context];
  } else {
    NSArray<AVCaptureDevice *> *devices = [AVCaptureDevice devices];
    SnlfAVCaptureRegisterDevices(context, devices);
    [devices release];

    NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
    NSOperationQueue *operationQueue = [NSOperationQueue mainQueue];
    id<NSObject> connectedHandler = [notificationCenter addObserverForName:AVCaptureDeviceWasConnectedNotification object:nil queue:operationQueue usingBlock:^(NSNotification *notification) {
    }];
    id<NSObject> disconnectedHandler = [notificationCenter addObserverForName:AVCaptureDeviceWasDisconnectedNotification object:nil queue:operationQueue usingBlock:^(NSNotification *notification) {

    }];

    context->connectHandler = connectedHandler;
    context->disconnectHandler = disconnectedHandler;
  }
#elif TARGET_OS_IOS
  if (@available(iOS 10.0, *)) {
    NSArray<AVCaptureDeviceType> *deviceTypes;
    if (@available(iOS 13.0, *)) {
      deviceTypes = @[AVCaptureDeviceTypeBuiltInWideAngleCamera, AVCaptureDeviceTypeBuiltInTelephotoCamera, AVCaptureDeviceTypeBuiltInUltraWideCamera, AVCaptureDeviceTypeBuiltInTrueDepthCamera];
    } else if (@available(iOS 11.1, *)) {
      deviceTypes = @[AVCaptureDeviceTypeBuiltInWideAngleCamera, AVCaptureDeviceTypeBuiltInTelephotoCamera, AVCaptureDeviceTypeBuiltInTrueDepthCamera];
    } else {
      deviceTypes = @[AVCaptureDeviceTypeBuiltInWideAngleCamera, AVCaptureDeviceTypeBuiltInTelephotoCamera];
    }
    AVCaptureDeviceDiscoverySession *discoverySession = [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:deviceTypes mediaType:AVMediaTypeVideo position:AVCaptureDevicePositionUnspecified];
    SnlfAVCaptureRegisterDevices(context, discoverySession.devices);
    [discoverySession release];
  } else {
    NSArray<AVCaptureDevice *> *devices = [AVCaptureDevice devices];
    SnlfAVCaptureRegisterDevices(context, devices);
    [devices release];
  }
#endif
  return context;
}

bool SnlfModuleUnload(intptr_t _context) {
  struct SnlfAVCaptureModuleContext *context = (struct SnlfAVCaptureModuleContext *)_context;
  free(context);
  return false;
}
