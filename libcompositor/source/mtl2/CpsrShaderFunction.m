#include "CpsrGraphics+Private.h"

// clang-format off
static inline MTLLanguageVersion ShaderVersionAsMetalType(CpsrShaderType shaderType) {
  switch (shaderType & CPSR_SHADER_VERSION_MASK) {
#if TARGET_OS_IPHONE
  case CPSR_SHADER_VERSION_MSL1_0:  return MTLLanguageVersion1_0;
#endif
  case CPSR_SHADER_VERSION_MSL2_0:  return MTLLanguageVersion2_0;
  case CPSR_SHADER_VERSION_MSL2_1:  return MTLLanguageVersion2_1;
  case CPSR_SHADER_VERSION_MSL2_2:  return MTLLanguageVersion2_2;
  case CPSR_SHADER_VERSION_MSL1_1:
  default:                          return MTLLanguageVersion1_1;
  }
}
// clang-format on

static inline CpsrShaderFunction *ShaderFunctionCreate(id<MTLFunction> native) {
  CpsrShaderFunction *function = CpsrAlloc(CpsrShaderFunction);
  if (function) {
    function->native = native;
  }
  return function;
}

CpsrShaderFunction *CpsrShaderFunctionCreateFromLibrary(const CpsrShaderLibrary *shaderLibrary, const char *functionName) {
  NSString *nsFunctionName = [[NSString alloc] initWithBytesNoCopy:(void *)functionName length:strlen(functionName) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  id<MTLFunction> native = [shaderLibrary->native newFunctionWithName:nsFunctionName];
  
  [nsFunctionName release];
  
  if (native) {
    CpsrShaderFunction *function = ShaderFunctionCreate(native);
    return function;
  }
  return NULL;
}

// TODO: Macro support
CpsrShaderFunction *CpsrShaderFunctionCreateFromString(const CpsrDevice *device, const char *functionString, CpsrShaderType shaderType) {
  if ((shaderType & CPSR_SHADER_LANGUAGE_MASK) != CPSR_SHADER_MSL) {
    return NULL;
  }
  
  NSString *nsFunctionString = [[NSString alloc] initWithBytesNoCopy:(void *)functionString length:strlen(functionString) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  
  MTLCompileOptions *options = [MTLCompileOptions new];
  options.languageVersion = ShaderVersionAsMetalType(shaderType);
  
  NSError *error = nil;
  id<MTLLibrary> library = [device->native newLibraryWithSource:nsFunctionString options:options error:&error];
  
  [options release];
  [nsFunctionString release];
  
  if (library) {
    id<MTLFunction> native = [library newFunctionWithName:@"_main"];
    if (native) {
      CpsrShaderFunction *function = ShaderFunctionCreate(native);
      return function;
    }
  }
  
  if (error) {
    // TODO: Log
  }
  return NULL;
}

void CpsrShaderFunctionDestroy(CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(shaderFunction);
  
  [shaderFunction->native release];
  CpsrDealloc(shaderFunction);
}
