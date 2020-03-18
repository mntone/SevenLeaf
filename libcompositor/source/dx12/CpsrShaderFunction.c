#include "CpsrGraphics+Private.h"

// clang-format off
static inline void ShaderTypeAsDx12Type(CpsrShaderType shaderType, char out[7]) {
  switch (shaderType & CPSR_SHADER_TYPE_MASK) {
  case CPSR_SHADER_TYPE_PIXEL_SHADER:   out[0] = 'p'; break;
  case CPSR_SHADER_TYPE_COMPUTE_SHADER: out[0] = 'c'; break;
  case CPSR_SHADER_TYPE_VERTEX_SHADER:
  default:                              out[0] = 'v'; break;
  }
}

static inline void ShaderVersionAsDx12Type(CpsrShaderType shaderType, char out[7]) {
  switch (shaderType & CPSR_SHADER_VERSION_MASK) {
  case CPSR_SHADER_VERSION_HLSL5_1:  memcpy(out, "xs_5_1", 7); break;
  case CPSR_SHADER_VERSION_HLSL6_0:  memcpy(out, "xs_6_0", 7); break;
  case CPSR_SHADER_VERSION_HLSL6_1:  memcpy(out, "xs_6_1", 7); break;
  case CPSR_SHADER_VERSION_HLSL5_0:
  default:                           memcpy(out, "xs_5_0", 7); break;
  }
}
// clang-format on

#include <d3dcompiler.h>

CpsrShaderFunction *CpsrShaderFunctionCreateFromLibrary(const CpsrShaderLibrary *shaderLibrary,
                                                        const char *functionName) {
  size_t filepathLength = shaderLibrary->filepathLength;
  size_t filenameLength = filepathLength + strlen(functionName);
  char *savedFilename = (char *)malloc(sizeof(char) * (filenameLength + 1));
  memcpy(savedFilename, shaderLibrary->filepath, filepathLength + 1);
  strcat(savedFilename, functionName);

  ID3DBlob *native;
  HRESULT hr = D3DReadFileToBlob(savedFilename, &native);
  free(savedFilename);
  if (FAILED(hr)) {
    return NULL;
  }

  CpsrShaderFunction *function = CpsrAlloc(CpsrShaderFunction);
  if (function) {
    function->native = native;
  }
  return function;
}

// TODO: Macro support
CpsrShaderFunction *CpsrShaderFunctionCreateFromString(const CpsrDevice *device,
                                                       const char *functionString,
                                                       CpsrShaderType shaderType) {
  if ((shaderType & CPSR_SHADER_LANGUAGE_MASK) != CPSR_SHADER_HLSL) {
    return NULL;
  }

  char target[7];
  ShaderVersionAsDx12Type(shaderType, target);
  ShaderTypeAsDx12Type(shaderType, target);

  UINT flag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifndef NDEBUG
  flag = D3DCOMPILE_DEBUG | D3DCOMPILE_OPTIMIZATION_LEVEL0;
#else
  flag = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

  ID3DBlob *native;
  ID3DBlob *errorBlob;
  HRESULT hr = D3DCompile(
      functionString, strlen(functionString), NULL, NULL, NULL, "_main", target, flag, 0, &native, &errorBlob);
  if (FAILED(hr)) {
    // TODO: error log
    return NULL;
  }

  CpsrShaderFunction *function = CpsrAlloc(CpsrShaderFunction);
  if (function) {
    function->native = native;
  }
  return function;
}

void CpsrShaderFunctionDestroy(CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(shaderFunction);

  ID3D10Blob_Release(shaderFunction->native);
  CpsrDealloc(shaderFunction);
}
