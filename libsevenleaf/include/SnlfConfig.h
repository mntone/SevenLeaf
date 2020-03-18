#ifndef _SNLF_CONFIG_H
#define _SNLF_CONFIG_H

// clang-format off
#define SNLF_LIB_VERSION                 1
#define SNLF_API_VERSION                 1

#define SNLF_MIN_WIDTH                   16
#define SNLF_MIN_HEIGHT                  16
#define SNLF_MAX_WIDTH                   16384
#define SNLF_MAX_HEIGHT                  16384

#define SNLF_INPUT_IDENTIFIER_LENGTH     36
#define SNLF_INPUT_FRIENDLY_NAME_LENGTH  (60 - 1)

#define SNLF_OUTPUT_FRIENDLY_NAME_LENGTH 64

#define SNLF_OUTPUT_BUFFER_COUNT         4 // Use quad buffer
#define SNLF_INPUT_BUFFER_COUNT          4 // Use dynamic
#define SNLF_MAX_DISPLAY_COUNT           8

#define SNLF_DISPLAY_DEFAULT_PADDING 8.F

#ifdef _WIN32
#define SNLF_UI_WINDOW_PADDING 11
#elif __APPLE__
#define SNLF_UI_WINDOW_PADDING 20
#else
#define SNLF_UI_WINDOW_PADDING 12
#endif

// clang-format on

#ifdef _WIN32
#  ifdef libsevenleaf_EXPORTS
#    define SNLF_EXPORT __declspec(dllexport)
#  else
#    define SNLF_EXPORT __declspec(dllimport)
#  endif
#else
#  ifdef libsevenleaf_EXPORTS
#    define SNLF_EXPORT __attribute__((visibility("default")))
#  else
#    define SNLF_EXPORT
#  endif
#endif

#define SNLF_ALIGN(__SIZE__) ((__SIZE__) + 15) & ~15

#include <stdio.h>
#include <stdint.h>

#endif  // _SNLF_CONFIG_H
