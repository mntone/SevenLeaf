#include "compositor/CpsrUtils.h"
#include <pthread.h>
#import <Foundation/NSThread.h>

double CpsrGetCurrentRawThreadPriority() {
  NSThread *currentThread = [NSThread currentThread];
  double threadPriority = [currentThread threadPriority];
  return threadPriority;
}

enum CpsrThreadPriority CpsrGetCurrentThreadPriority() {
  double threadPriority = CpsrGetCurrentRawThreadPriority();
  if (threadPriority >= 0.9) {
    return CSPR_TP_REALTIME;
  } else if (threadPriority > 0.7) {
    return CSPR_TP_HIGHEST;
  } else if (threadPriority > 0.5) {
    return CSPR_TP_ABOVE_NORMAL;
  } else if (threadPriority < 0.5) {
    return CSPR_TP_BACKGROUND;
  } else {
    return CSPR_TP_DEFAULT;
  }
}

bool CpsrSetCurrentThreadPriority(enum CpsrThreadPriority threadPriority) {
  NSThread *currentThread = [NSThread currentThread];
  switch (threadPriority) {
  case CSPR_TP_BACKGROUND:
    [currentThread setThreadPriority:0.3];
    break;
  case CSPR_TP_ABOVE_NORMAL:
    [currentThread setThreadPriority:0.6];
    break;
  case CSPR_TP_HIGHEST:
    [currentThread setThreadPriority:0.7];
    break;
  case CSPR_TP_REALTIME:
    [currentThread setThreadPriority:1.0];
    break;
  case CSPR_TP_DEFAULT:
  default:
    [currentThread setThreadPriority:0.5];
    break;
  }
  return true;
}
