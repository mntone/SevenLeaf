#include "CpsrTypedefs.h"

#ifndef _CPSR_UTILS_H
#define _CPSR_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

// CPU

enum CpsrThreadPriority {
  CSPR_TP_BACKGROUND = -2,
  CSPR_TP_DEFAULT = 0,
  CSPR_TP_ABOVE_NORMAL = 1,
  CSPR_TP_GRAPHICS = CSPR_TP_ABOVE_NORMAL,
  CSPR_TP_HIGHEST = 2,
  CSPR_TP_REALTIME = 15,
  CSPR_TP_SOUND = CSPR_TP_REALTIME,
};
CPSR_EXPORT enum CpsrThreadPriority CpsrGetCurrentThreadPriority();
#if defined(_WIN32)
CPSR_EXPORT double CpsrGetCurrentRawThreadPriority();
#elif defined(__APPLE__)
CPSR_EXPORT double CpsrGetCurrentRawThreadPriority();
#endif
CPSR_EXPORT bool CpsrSetCurrentThreadPriority(enum CpsrThreadPriority threadPriority);

#ifdef __cplusplus
}
#endif

#endif // _CPSR_GRAPHICS_H
