#import <AVFoundation/AVFoundation.h>

#import "SnlfAVDevicesObserver.h"

@implementation SnlfAVDevicesObserver {
  AVCaptureDeviceDiscoverySession *_discoverySession;
}

- (instancetype)initWithSession:(AVCaptureDeviceDiscoverySession *)discoverySession context:(nullable struct SnlfAVCaptureModuleContext *)context {
  if (self = [super init]) {
    _discoverySession = discoverySession;
    [self addObserver:self forKeyPath:@"devices" options:NSKeyValueObservingOptionNew context:context];
  }
  return self;
}

- (void)dealloc {
  [self removeObserver:self forKeyPath:@"devices"];
  [_discoverySession release];
  [super dealloc];
}

- (void)observeValueForKeyPath:(nullable NSString *)keyPath ofObject:(nullable id)object change:(nullable NSDictionary<NSKeyValueChangeKey, id> *)change context:(nullable void *)context {
  change[@"new"];
}

@end
