#ifndef _SNLF_LOG_H
#define _SNLF_LOG_H

#include "SnlfConfig.h"

#include <stdarg.h>

enum _SnlfLogLevel {
  SNLF_LOG_VERBOSE =   0,
  SNLF_LOG_DEBUG   = 100,
  SNLF_LOG_INFO    = 200,
  SNLF_LOG_WARNING = 300,
  SNLF_LOG_ERROR   = 400,
  SNLF_LOG_ASSERT  = 500,
};
typedef enum _SnlfLogLevel SnlfLogLevel;

typedef void (*SnlfLogHandler)(SnlfLogLevel logLevel, const char *format, va_list args, void *param);

SNLF_EXPORT void SnlfSetLogHandler(SnlfLogHandler logHandler, void *param);

SNLF_EXPORT void _SnlfLogFormat(SnlfLogLevel logLevel, const char *format, ...);

#define SnlfLogFormat(__LEVEL__, __FORMAT__, ...)  _SnlfLogFormat(__LEVEL__, __FORMAT__ " (line %d of file " __FILEPATH__ ", function %s)", __VA_ARGS__, __LINE__, __FUNCTION__)
#define SnlfVerboseLogFormat(__FORMAT__, ...)      SnlfLogFormat(SNLF_LOG_VERBOSE, __FORMAT__, __VA_ARGS__)
#define SnlfDebugLogFormat(__FORMAT__, ...)        SnlfLogFormat(SNLF_LOG_DEBUG, __FORMAT__, __VA_ARGS__)
#define SnlfInfoLogFormat(__FORMAT__, ...)         SnlfLogFormat(SNLF_LOG_INFO, __FORMAT__, __VA_ARGS__)
#define SnlfWarningLogFormat(__FORMAT__, ...)      SnlfLogFormat(SNLF_LOG_WARNING, __FORMAT__, __VA_ARGS__)
#define SnlfErrorLogFormat(__FORMAT__, ...)        SnlfLogFormat(SNLF_LOG_ERROR, __FORMAT__, __VA_ARGS__)
#define SnlfAssertFormat(__FORMAT__, ...)          SnlfLogFormat(SNLF_LOG_ASSERT, __FORMAT__, __VA_ARGS__)

#define SnlfLog(__LEVEL__, __MESSAGE__) _SnlfLogFormat(__LEVEL__, __MESSAGE__ " (line %d of file " __FILEPATH__ ", function %s)", __LINE__, __FUNCTION__)
#define SnlfVerboseLog(__MESSAGE__)     SnlfLog(SNLF_LOG_VERBOSE, __MESSAGE__)
#define SnlfDebugLog(__MESSAGE__)       SnlfLog(SNLF_LOG_DEBUG, __MESSAGE__)
#define SnlfInfoLog(__MESSAGE__)        SnlfLog(SNLF_LOG_INFO, __MESSAGE__)
#define SnlfWarningLog(__MESSAGE__)     SnlfLog(SNLF_LOG_WARNING, __MESSAGE__)
#define SnlfErrorLog(__MESSAGE__)       SnlfLog(SNLF_LOG_ERROR, __MESSAGE__)
#define SnlfAssertLog(__MESSAGE__)      SnlfLog(SNLF_LOG_ASSERT, __MESSAGE__)

#define SnlfOutOfMemoryError()                 SnlfAssertLog("Out of memory, malloc failed.")
#define SnlfAssertWithOutOfMemory(__MESSAGE__) SnlfAssertFormat("%s, maybe out of memory.", __MESSAGE__)

#define SnlfPixelFormatError()                 SnlfAssertLog("Pixel format setting failed.")

#define SnlfMutexCreationError()               SnlfAssertLog("Mutex creation failed.")
#define SnlfMutexCreationDisposingError()      SnlfWarningLog("Mutexattr disposing failed.")
#define SnlfMutexLockError()                   SnlfAssertLog("Mutex lock failed.")
#define SnlfMutexUnlockError()                 SnlfAssertLog("Mutex unlock failed.")
#define SnlfMutexDestructionError()            SnlfWarningLog("Mutex destruction failed.")

#endif // _SNLF_LOG_H
