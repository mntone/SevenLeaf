#include "CpsrGraphics+Private.h"

CpsrShaderLibrary *CpsrShaderLibraryCreate(const CpsrDevice *device, const char *filePath) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(filePath);
  
  NSMutableString *nsFilePath = [[NSMutableString alloc] initWithBytesNoCopy:(void *)filePath length:strlen(filePath) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  [nsFilePath appendString:@"default.metallib"];
  
  NSError *error = nil;
  id<MTLLibrary> native = [device->native newLibraryWithFile:nsFilePath error:&error];
  
  [nsFilePath release];
  
  if (!native) {
    [error release];
    // TODO: Log
    return NULL;
  }

  CpsrShaderLibrary *shaderLibrary = CpsrAlloc(CpsrShaderLibrary);
  if (shaderLibrary) {
    shaderLibrary->device = device;
    shaderLibrary->native = native;
  }
  return shaderLibrary;
}

void CpsrShaderLibraryDestroy(CpsrShaderLibrary *shaderLibrary) {
  CPSR_ASSUME(shaderLibrary);
  assert(shaderLibrary->native.retainCount == 1);
  
  [shaderLibrary->native release];
  CpsrDealloc(shaderLibrary);
}
