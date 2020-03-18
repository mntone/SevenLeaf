#ifndef _SNLF_AVDEVICES_OBSERVER_H
#define _SNLF_AVDEVICES_OBSERVER_H

struct SnlfAVCaptureModuleContext;

NS_ASSUME_NONNULL_BEGIN

@interface SnlfAVDevicesObserver : NSObject

- (instancetype)initWithSession:(AVCaptureDeviceDiscoverySession *)discoverySession context:(nullable struct SnlfAVCaptureModuleContext *)context;
- (void)observeValueForKeyPath:(nullable NSString *)keyPath ofObject:(nullable id)object change:(nullable NSDictionary<NSKeyValueChangeKey, id> *)change context:(nullable void *)context;

@end

NS_ASSUME_NONNULL_END

#endif
