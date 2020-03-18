#ifndef _SNLF_DISPLAY_H
#define _SNLF_DISPLAY_H

#include "SnlfCore.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---
// Prototypes
// ---
typedef struct _SnlfDisplayContext SnlfDisplayContext;

typedef void (*SnlfDisplayContextDrawHandler)(intptr_t, const SnlfDisplayContext *);

// ---
// Functions
// ---
SNLF_EXPORT SnlfDisplay *SnlfDisplayCreate(SnlfCoreRef core, CpsrViewHost viewHost, SnlfDisplayContextDrawHandler handler, intptr_t param);
SNLF_EXPORT void SnlfDisplayDestroy(SnlfDisplay *display);

SNLF_EXPORT void SnlfDisplayChangeSize(SnlfDisplay *display, CpsrSizeU32 size);
SNLF_EXPORT void SnlfDisplayBeginDraw(SnlfDisplay *display);
SNLF_EXPORT void SnlfDisplayEndDraw(SnlfDisplay *display);

SNLF_EXPORT const SnlfGraphicsContext *SnlfDisplayContextGetGraphicsContext(const SnlfDisplayContext *displayContext);
SNLF_EXPORT const CpsrTexture2D *SnlfDisplayContextGetSourceTexture(const SnlfDisplayContext *displayContext);
SNLF_EXPORT CpsrSwapChain *SnlfDisplayContextGetSwapChain(const SnlfDisplayContext *displayContext);

SNLF_EXPORT void SnlfDisplayContextPresent(const SnlfDisplayContext *context);

#ifdef __cplusplus
}
#endif

#endif // _SNLF_MODULE_H
