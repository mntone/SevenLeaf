#ifndef _OSUTIL_STRING_H
#define _OSUTIL_STRING_H

#include "osutil_base.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <CoreFoundation/CFString.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t osutil_string_length_t;

struct _osutil_string {
  char *data;
  osutil_string_length_t size, capacity;
};
typedef struct _osutil_string osutil_string_t;

static inline void osutil_string_init(osutil_string_t *string) {
  assert(string);
  assert(string->data);

  string->data = NULL;
  string->size = 0;
  string->capacity = 0;
}

static inline osutil_string_length_t osutil_string_reserve(osutil_string_t *string,
                                                           osutil_string_length_t new_capacity) {
  assert(string);

  if (new_capacity <= string->capacity) {
    return string->capacity;
  }

  osutil_string_length_t target_capacity = (new_capacity + 15) & ~15;
  char *temp = (char *)realloc(string->data, target_capacity);
  if (!temp) {
    return string->capacity;
  }
  
  string->data     = temp;
  string->capacity = target_capacity;
  return target_capacity;
}

static inline bool osutil_string_copy(osutil_string_t *string, const char *data) {
  assert(string);
  assert(!string->data);
  assert(data);

  const osutil_string_length_t length = (osutil_string_length_t)strlen(data);
  const osutil_string_length_t max_size = length + 1;
  if (osutil_string_reserve(string, max_size) < max_size) {
    return true;
  }
  memcpy(string->data, data, max_size);

  string->size = length;
  return false;
}

static inline bool osutil_string_init_with_data(osutil_string_t *string, const char *data) {
  assert(string);
  assert(!string->data);
  assert(data);

  return osutil_string_copy(string, data);
}

#ifdef _WIN32
OSUTIL_EXPORT void osutil_string_copy_from_wcstring(osutil_string_t *string, const wchar_t *wcs);
OSUTIL_EXPORT void osutil_string_copy_from_wcstring_and_length(osutil_string_t *string,
                                                               const wchar_t *wcs,
                                                               osutil_string_length_t wcs_length);
#endif

#ifdef __APPLE__
static inline bool osutil_string_copy_with_cfstring(osutil_string_t *string, const CFStringRef cf_string) {
  assert(string);
  assert(!string->data);
  assert(cf_string);

  CFIndex length = CFStringGetLength(cf_string);
  CFIndex max_size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
  if (osutil_string_reserve(string, max_size) < max_size) {
    return true;
  }
  if (!CFStringGetCString(cf_string, string->data, string->capacity, kCFStringEncodingUTF8)) {
    return true;
  }
  string->size = strlen(string->data);
  return false;
}
#endif

static inline void osutil_string_free(osutil_string_t *string) {
  assert(string);
  assert(string->data);

  free(string->data);
  osutil_string_init(string);
}

static inline bool osutil_string_endswith(osutil_string_t *string, const char *suffix) {
  assert(string);
  assert(string->data);
  assert(suffix);

  const osutil_string_length_t suffix_length = (osutil_string_length_t)strlen(suffix);
  if (string->size < suffix_length) {
    return false;
  }
  return strncmp(string->data + string->size - suffix_length, suffix, suffix_length) == 0;
}

static inline bool osutil_string_join(osutil_string_t *string, const char *data) {
  assert(string);
  assert(string->data);
  assert(data);

  const osutil_string_length_t join_size = (osutil_string_length_t)strlen(data);
  const osutil_string_length_t new_size = string->size + join_size;
  const osutil_string_length_t max_size = new_size + 1;
  if (osutil_string_reserve(string, max_size) < max_size) {
    return true;
  }
  memcpy(string->data + string->size, data, join_size);

  string->data[new_size] = '\0';
  string->size = new_size;
  return false;
}

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_STRING_H
