#include <CoreGraphics/CGDirectDisplay.h>
#include "CpsrGraphics+Private.h"

#include <assert.h>
#include <string.h>

#if TARGET_OS_IPHONE
#include <mach/task_info.h>
#endif

CpsrDriverType CpsrGetDriverType() {
  return CPSR_DRIVER_METAL2;
}

static inline CpsrDevice *CpsrDeviceCreate(id<MTLDevice> native) {
  CpsrDevice *device = CpsrAlloc(CpsrDevice);
  if (device) {
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
    if (@available(iOS 13.0, macOS 10.15, *)) {
      device->hasUnifiedMemory = native.hasUnifiedMemory;
    } else {
      device->hasUnifiedMemory = [native.name containsString:@"Intel"];
    }
#else
    device->hasUnifiedMemory = true;
#endif
    device->native = native;
  }
  return device;
}

void CpsrEnumDevice(CpsrDeviceProcedure enumFunc) {
  NSArray *devices = MTLCopyAllDevices();
  for (NSUInteger i = 0; i < devices.count; ++i) {
    id<MTLDevice> native = devices[i];
    CpsrDevice *device = CpsrDeviceCreate(native);
    if (device) {
      enumFunc(device);
    }
  }
  [devices release];
}

CpsrDevice *CpsrDeviceGetDefault() {
  id<MTLDevice> nativeDevice = MTLCreateSystemDefaultDevice();
  if (!nativeDevice) {
    return NULL;
  }

  CpsrDevice *device = CpsrDeviceCreate(nativeDevice);
  return device;
}

CpsrDevice *CpsrDeviceGetLowPower() {
  NSArray *devices = MTLCopyAllDevices();
  for (NSUInteger i = 0; i < devices.count; ++i) {
    id<MTLDevice> native = devices[i];
    if (native.isLowPower) {
      CpsrDevice *device = CpsrDeviceCreate(native);
      return device;
    }
    [native release];
  }
  [devices release];

  return CpsrDeviceGetDefault();
}

CpsrDevice *CpsrDeviceGetPreferred(CGDirectDisplayID displayID) {
  id<MTLDevice> nativeDevice = CGDirectDisplayCopyCurrentMetalDevice(displayID);
  if (!nativeDevice) {
    return NULL;
  }

  CpsrDevice *device = CpsrDeviceCreate(nativeDevice);
  return device;
}

void CpsrDeviceDestroy(CpsrDevice *device) {
  CPSR_ASSUME(device);
  assert(device->native.retainCount == 1);

  [device->native release];
  CpsrDealloc(device);
}

size_t CpsrDeviceGetName(const CpsrDevice *device, char *deviceName, size_t maxCount) {
  CPSR_ASSUME(device);

  if (deviceName) {
    if (!CFStringGetCString((CFStringRef)device->native.name, deviceName, maxCount, kCFStringEncodingUTF8)) {
      return 0;
    }
    return strnlen(deviceName, maxCount);
  } else {
    return strlen(device->native.name.UTF8String);
  }
}

uint64_t CpsrGraphicsDeviceGetCurrentAllocatedSize(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  if (@available(iOS 11.0, macOS 10.13, *)) {
    return device->native.currentAllocatedSize;
  } else {
#if TARGET_OS_IOS
    task_vm_info_data_t taskInfo;
    mach_msg_type_number_t taskSize = TASK_VM_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_VM_INFO, (task_info_t)&taskInfo, &taskSize) == KERN_SUCCESS) {
      return taskInfo.phys_footprint;
    }
#endif
    return 0;
  }
}

bool CpsrDeviceIsUnifiedMemoryAccess(const CpsrDevice *device) {
  return device->hasUnifiedMemory;
}

bool CpsrDeviceIsMultisampleSupport(const CpsrDevice *device, CpsrPixelFormat pixelFormat, uint8_t sampleCount) {
  return [device->native supportsTextureSampleCount:sampleCount];
}

extern inline bool MtlDeviceSupportsFamilyBackport(id<MTLDevice> mtlDevice, MTLGPUFamily family) {
#if TARGET_OS_IOS
  if (@available(iOS 13.0, *)) {
    return [mtlDevice supportsFamily:family];
  } else if (@available(iOS 12.0, *)) {
    switch (family) {
    case MTLGPUFamilyApple4:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily4_v2];
        
    case MTLGPUFamilyApple3:
    case MTLGPUFamilyCommon2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v4];

    case MTLGPUFamilyApple2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v5];

    case MTLGPUFamilyApple1:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v5];

    default:
      break;
    }
  } else if (@available(iOS 11.0, *)) {
    switch (family) {
    case MTLGPUFamilyApple4:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily4_v1];
        
    case MTLGPUFamilyApple3:
    case MTLGPUFamilyCommon2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v3];

    case MTLGPUFamilyApple2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v4];

    case MTLGPUFamilyApple1:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v4];

    default:
      break;
    }
  } else if (@available(iOS 10.0, *)) {
    switch (family) {
    case MTLGPUFamilyApple3:
    case MTLGPUFamilyCommon2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v2];

    case MTLGPUFamilyApple2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v3];

    case MTLGPUFamilyApple1:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v3];

    default:
      break;
    }
  } else if (@available(iOS 9.0, *)) {
    switch (family) {
    case MTLGPUFamilyApple3:
    case MTLGPUFamilyCommon2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1];

    case MTLGPUFamilyApple2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v2];
    
    case MTLGPUFamilyApple1:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v2];

    default:
      break;
    }
  } else {
    switch (family) {
    case MTLGPUFamilyApple2:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1];
        
    case MTLGPUFamilyApple1:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v1];
        
    default:
      break;
    }
  }
#elif TARGET_OS_OSX || TARGET_OS_MACCATALYST
  if (@available(macOS 10.15, *)) {
    return [mtlDevice supportsFamily:family];
  } else if (@available(macOS 10.14, *)) {
    switch (family) {
    case MTLGPUFamilyMac2:
    case MTLGPUFamilyCommon3:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily2_v1];
        
    case MTLGPUFamilyMac1:
    case MTLGPUFamilyCommon2:
    case MTLGPUFamilyCommon1:
      return [mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v4];
    
    default:
      break;
    }
  } else {
    switch (family) {
    case MTLGPUFamilyMac1:
    case MTLGPUFamilyCommon2:
    case MTLGPUFamilyCommon1:
      if (@available(macOS 10.13, *)) {
        return [mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v3];
      } else if (@available(macOS 10.12, *)) {
        return [mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v2];
      } else {
        return [mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v1];
      }

    default:
      break;
    }
  }
#endif
  return false;
}

extern inline void CpsrPixelFormatCapabilitiesSetSelect(CpsrPixelFormatCapabilities *capabilities, bool sample, bool write, bool renderTarget, bool multisample, bool blend) {
  capabilities->load = true;
  if (sample) {
    capabilities->sample = true;
  }
  capabilities->mipmap = true;
  if (renderTarget) {
    capabilities->renderTarget = true;
  }
  if (blend) {
    capabilities->blend = true;
  }
  if (write) {
    capabilities->write = true;
  }
  if (multisample) {
    capabilities->multisampleLoad = true;
    if (sample) {
      capabilities->multisampleSample = true;
    }
  }
}

extern inline void CpsrPixelFormatCapabilitiesSet_Write_RenderTarget(CpsrPixelFormatCapabilities *capabilities) {
  capabilities->load = true;
  capabilities->mipmap = true;
  capabilities->renderTarget = true;
  capabilities->write = true;
}

extern inline void CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(CpsrPixelFormatCapabilities *capabilities) {
  CpsrPixelFormatCapabilitiesSet_Write_RenderTarget(capabilities);
  capabilities->multisampleLoad = true;
}

extern inline void CpsrPixelFormatCapabilitiesSetAllExceptWrite(CpsrPixelFormatCapabilities *capabilities) {
  capabilities->load = true;
  capabilities->sample = true;
  capabilities->mipmap = true;
  capabilities->renderTarget = true;
  capabilities->blend = true;
  capabilities->multisampleLoad = true;
  capabilities->multisampleSample = true;
  capabilities->multisampleResolve = true;
}

extern inline void CpsrPixelFormatCapabilitiesSetAllExceptResolve(CpsrPixelFormatCapabilities *capabilities) {
  capabilities->load = true;
  capabilities->sample = true;
  capabilities->mipmap = true;
  capabilities->renderTarget = true;
  capabilities->blend = true;
  capabilities->write = true;
  capabilities->multisampleLoad = true;
  capabilities->multisampleSample = true;
}

extern inline void CpsrPixelFormatCapabilitiesSetAll(CpsrPixelFormatCapabilities *capabilities) {
  CpsrPixelFormatCapabilitiesSetAllExceptResolve(capabilities);
  capabilities->multisampleResolve = true;
}

CpsrPixelFormatCapabilities CpsrDeviceGetPixelFormatCapabilities(const CpsrDevice *device, CpsrPixelFormat pixelFormat) {
  CpsrPixelFormatCapabilities capabilities = {0};
  switch (pixelFormat) {
  // 8-bit
  case CPSR_PIXELFORMAT_R8_UNORM:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_R8_SNORM:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;
      
  case CPSR_PIXELFORMAT_R8_UINT:
  case CPSR_PIXELFORMAT_R8_SINT:
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;
  
  case CPSR_PIXELFORMAT_A8_UNORM:
    CpsrPixelFormatCapabilitiesSetSelect(&capabilities, true, false, false, false, false);
    break;
      
  // 16-bit
  case CPSR_PIXELFORMAT_R16_UNORM:
  case CPSR_PIXELFORMAT_R16_SNORM:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;
      
  case CPSR_PIXELFORMAT_R16_UINT:
  case CPSR_PIXELFORMAT_R16_SINT:
      CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_R16_FLOAT:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
  
  case CPSR_PIXELFORMAT_RG8_UNORM:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;

  case CPSR_PIXELFORMAT_RG8_SNORM:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RG8_UINT:
  case CPSR_PIXELFORMAT_RG8_SINT:
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;
  
#if TARGET_OS_IOS
  case CPSR_PIXELFORMAT_ABGR4_UNORM:
  case CPSR_PIXELFORMAT_B5G6R5_UNORM:
  case CPSR_PIXELFORMAT_A1BGR5_UNORM:
  case CPSR_PIXELFORMAT_BGR5A1_UNORM:
    CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
    break;
#endif
      
  // 32-bit
  case CPSR_PIXELFORMAT_R32_UINT:
  case CPSR_PIXELFORMAT_R32_SINT:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget(&capabilities);
#else
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
#endif
    break;
      
  case CPSR_PIXELFORMAT_R32_FLOAT:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetSelect(&capabilities, false, true, true, true, true);
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RG16_UNORM:
  case CPSR_PIXELFORMAT_RG16_SNORM:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;
  
  case CPSR_PIXELFORMAT_RG16_UINT:
  case CPSR_PIXELFORMAT_RG16_SINT:
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_RG16_FLOAT:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_RGBA8_UNORM:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_RGBA8_UNORM_SRGB:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
#endif
    break;
      
  case CPSR_PIXELFORMAT_RGBA8_SNORM:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RGBA8_UINT:
  case CPSR_PIXELFORMAT_RGBA8_SINT:
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;

  case CPSR_PIXELFORMAT_BGRA8_UNORM:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
  
  case CPSR_PIXELFORMAT_BGRA8_UNORM_SRGB:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RGB10A2_UNORM:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple3)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
    }
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;
  
  case CPSR_PIXELFORMAT_RGB10A2_UINT:
#if TARGET_OS_IOS
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple3)) {
      CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetSelect(&capabilities, false, false, true, true, false);
    }
#else
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_BGR10A2_UNORM:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#else
    if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyMac2)) {
      CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    } else {
      CpsrPixelFormatCapabilitiesSetAllExceptWrite(&capabilities);
    }
#endif
    break;

  // 64-bit
  case CPSR_PIXELFORMAT_RG32_UINT:
  case CPSR_PIXELFORMAT_RG32_SINT:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget(&capabilities);
#else
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RG32_FLOAT:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetSelect(&capabilities, false, true, true, false, true);
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;

  case CPSR_PIXELFORMAT_RGBA16_UNORM:
  case CPSR_PIXELFORMAT_RGBA16_SNORM:
#if TARGET_OS_IOS
    CpsrPixelFormatCapabilitiesSetAllExceptResolve(&capabilities);
#else
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
#endif
    break;
      
  case CPSR_PIXELFORMAT_RGBA16_UINT:
  case CPSR_PIXELFORMAT_RGBA16_SINT:
    CpsrPixelFormatCapabilitiesSet_Write_RenderTarget_Multisample(&capabilities);
    break;
      
  case CPSR_PIXELFORMAT_RGBA16_FLOAT:
    CpsrPixelFormatCapabilitiesSetAll(&capabilities);
    break;
  
  default:
    break;
  }
  return capabilities;
}

CpsrDeviceCapabilities CpsrDeviceGetCapabilities(const CpsrDevice *device) {
  CpsrDeviceCapabilities capabilities;
#if TARGET_OS_IOS
  capabilities.bufferOffsetAlignment = 4;
  if (MtlDeviceSupportsFamilyBackport(device->native, MTLGPUFamilyApple2)) {
    capabilities.maximumRenderTargetCount = 8;
  } else {
    capabilities.maximumRenderTargetCount = 4;
  }
#else
  capabilities.bufferOffsetAlignment = 256;
  capabilities.maximumRenderTargetCount = 8;
#endif
  return capabilities;
}

id<MTLDevice> CpsrDeviceGetNativeHandle(const CpsrDevice *device) {
  return device->native;
}
