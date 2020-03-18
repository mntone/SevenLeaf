#include "SnlfLog.h"

#ifdef NDEBUG
#define SNLF_CONSOLE_LOG_LEVEL SNLF_LOG_INFO
#else
#define SNLF_CONSOLE_LOG_LEVEL SNLF_LOG_VERBOSE
#endif

void _SnlfDefaultLogHandler(SnlfLogLevel logLevel, const char *format, va_list args, void *param) {
  if (logLevel < SNLF_CONSOLE_LOG_LEVEL) {
    return;
  }
  
  char out[1024];
  vsnprintf(out, sizeof(out), format, args);
  
  switch (logLevel) {
  case SNLF_LOG_VERBOSE:
    fprintf(stdout, "Verbose: %s\n", out);
    fflush(stdout);
    break;
      
  case SNLF_LOG_DEBUG:
    fprintf(stdout, "Debug: %s\n", out);
    fflush(stdout);
    break;

  case SNLF_LOG_INFO:
    fprintf(stdout, "Info: %s\n", out);
    fflush(stdout);
    break;

  case SNLF_LOG_WARNING:
    fprintf(stdout, "Warning: %s\n", out);
    fflush(stdout);
    break;

  case SNLF_LOG_ERROR:
    fprintf(stderr, "Error: %s\n", out);
    fflush(stderr);
    break;

  case SNLF_LOG_ASSERT:
    fprintf(stderr, "Assert: %s\n", out);
    fflush(stderr);
  }
}

static SnlfLogHandler gCurrentLogHandler = _SnlfDefaultLogHandler;
static void *gParam = NULL;

void SnlfSetLogHandler(SnlfLogHandler logHandler, void *param) {
  if (logHandler) {
    gCurrentLogHandler = logHandler;
    gParam = param;
  } else {
    gCurrentLogHandler = _SnlfDefaultLogHandler;
    gParam = NULL;
  }
}

static inline void _SnlfLogFormatVa(SnlfLogLevel logLevel, const char *format, va_list args) {
  gCurrentLogHandler(logLevel, format, args, NULL);
}

void _SnlfLogFormat(SnlfLogLevel logLevel, const char *format, ...) {
  va_list args;
  va_start(args, format);
  _SnlfLogFormatVa(logLevel, format, args);
  va_end(args);
}
