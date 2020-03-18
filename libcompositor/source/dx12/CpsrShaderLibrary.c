#include "CpsrGraphics+Private.h"

CpsrShaderLibrary *CpsrShaderLibraryCreate(const CpsrDevice *device, const char *filePath) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(filePath);

  size_t filePathLength = strlen(filePath);
  char *savedFilePath = (char *)malloc(sizeof(char) * (filePathLength + 1));
  if (!savedFilePath) {
    return NULL;
  }
  memcpy(savedFilePath, filePath, filePathLength);
  savedFilePath[filePathLength] = '\0';

  CpsrShaderLibrary *shaderLibrary = CpsrAlloc(CpsrShaderLibrary);
  if (shaderLibrary) {
    shaderLibrary->device = device;
    shaderLibrary->filepath = savedFilePath;
    shaderLibrary->filepathLength = filePathLength;
  }
  return shaderLibrary;
}

void CpsrShaderLibraryDestroy(CpsrShaderLibrary *shaderLibrary) {
  CPSR_ASSUME(shaderLibrary);

  free(shaderLibrary->filepath);
  CpsrDealloc(shaderLibrary);
}
