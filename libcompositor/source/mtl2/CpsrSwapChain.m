#import <QuartzCore/CAMetalLayer.h>

#define USE_SWAPCHAIN_PRIVATE_DEFINE 1
#include "CpsrGraphics+Private.h"

#import <AppKit/NSView.h>

CpsrSwapChain *CpsrSwapChainCreate(const CpsrCommandQueue *graphicsCommandQueue, CpsrViewHost viewHost, uint8_t bufferCount, bool vsyncEnable) {
  CPSR_ASSUME(graphicsCommandQueue);
  CPSR_ASSUME(bufferCount >= 2 && bufferCount <= 3);
  
  CpsrSwapChain *swapChain = CpsrAlloc(CpsrSwapChain);
  if (swapChain) {
    CAMetalLayer *metalLayer = [CAMetalLayer layer];
    metalLayer.device = graphicsCommandQueue->device->native;
    metalLayer.pixelFormat = PixelFormatAsMetalType(viewHost.pixelFormat);
    metalLayer.drawableSize = CGSizeMake(viewHost.size.width, viewHost.size.height);
    if (!vsyncEnable) {
      if (@available(macOS 10.13, *)) {
          metalLayer.displaySyncEnabled = NO;
      }
    }
    metalLayer.maximumDrawableCount = bufferCount;
    
    NSView *view = (NSView *)viewHost.handle;
    [view retain];
    view.wantsLayer = YES;
    view.layer = metalLayer;
    
    swapChain->graphicsCommandQueue = graphicsCommandQueue;
    swapChain->metalLayer           = metalLayer;
    swapChain->hostView             = (CpsrViewHandle)view;
    swapChain->currentDrawable      = nil;
  }
  return swapChain;
}

static inline void CpsrSwapChainReleaseDrawable(CpsrSwapChain *swapChain) {
  if (swapChain->currentDrawable) {
    [swapChain->currentDrawable release];
    swapChain->currentDrawable = nil;
  }
}

void CpsrSwapChainDestroy(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);
  
  NSView *view = (NSView *)swapChain->hostView;
  view.layer = nil;
  [view release];
  
  [swapChain->metalLayer release];
  CpsrDealloc(swapChain);
}

bool CpsrSwapChainNextBuffer(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);

  id<CAMetalDrawable> nextDrawable = [swapChain->metalLayer nextDrawable];
  swapChain->currentDrawable = nextDrawable;
  return !nextDrawable;
}

// ---
// Property: Size
// ---
CpsrSizeU32 CpsrSwapChainGetSize(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);
  
  CGSize nativeSize = swapChain->metalLayer.drawableSize;
  
  CpsrSizeU32 size;
  size.width = (uint32_t)nativeSize.width;
  size.height = (uint32_t)nativeSize.height;
  return size;
}

void CpsrSwapChainSetSize(CpsrSwapChain *swapChain, CpsrSizeU32 size) {
  CPSR_ASSUME(swapChain);
  
  swapChain->metalLayer.drawableSize = CGSizeMake(size.width, size.height);
}

// ---
// Property: Buffer count
// ---
uint8_t CpsrSwapChainGetBufferCount(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);
  
  return (uint8_t)swapChain->metalLayer.maximumDrawableCount;
}

void CpsrSwapChainSetBufferCount(CpsrSwapChain *swapChain, uint8_t bufferCount) {
  CPSR_ASSUME(swapChain);
  CPSR_ASSUME(bufferCount >= 2 && bufferCount <= 3);
  
  swapChain->metalLayer.maximumDrawableCount = bufferCount;
}

void CpsrSwapChainSetColorSpace(CpsrSwapChain *swapChain, CpsrColorSpace colorSpace) {
  CPSR_ASSUME(swapChain);
  
  CGColorSpaceRef nativeColorSpace;
  switch (colorSpace) {
  case CPSR_COLORSPACE_SRGB:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
    break;

  case CPSR_COLORSPACE_SRGB_LINEAR:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceLinearSRGB);
    break;

  case CPSR_COLORSPACE_ADOBERGB:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceAdobeRGB1998);
    break;

  case CPSR_COLORSPACE_BT709:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceITUR_709);
    break;

  case CPSR_COLORSPACE_DISPLAYP3:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceDisplayP3);
    break;

  case CPSR_COLORSPACE_DISPLAYP3_HLG:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceDisplayP3_HLG);
    break;

  case CPSR_COLORSPACE_DISPLAYP3_PQ:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceDisplayP3_PQ_EOTF);
    break;

  case CPSR_COLORSPACE_BT2020:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceITUR_2020);
    break;

  case CPSR_COLORSPACE_BT2020_HLG:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceITUR_2020_HLG);
    break;

  case CPSR_COLORSPACE_BT2020_PQ:
    nativeColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceITUR_2020_PQ_EOTF);
    break;
      
  case CPSR_COLORSPACE_DEFAULT:
  default:
    nativeColorSpace = NULL;
    break;
  }
  swapChain->metalLayer.colorspace = nativeColorSpace;
  if (nativeColorSpace) {
    CGColorSpaceRelease(nativeColorSpace);
  }
}

// ---
// Internal functions
// ---
id<MTLTexture> CpsrSwapChainGetNativeTexture(const CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);
  
  return swapChain->currentDrawable.texture;
}
