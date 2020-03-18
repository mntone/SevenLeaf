#include "osutil_string.h"

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void osutil_string_copy_from_wcstring(osutil_string_t* string, const wchar_t* wcs) {
  assert(string);
  assert(wcs);

  int utf8_length = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, NULL, 0, NULL, NULL);
  osutil_string_reserve(string, utf8_length + 1);
  string->size = WideCharToMultiByte(CP_UTF8, 0, wcs, -1, string->data, string->capacity, NULL, NULL);
}

void osutil_string_copy_from_wcstring_and_length(osutil_string_t* string,
                                                 const wchar_t* wcs,
                                                 osutil_string_length_t wcs_length) {
  assert(string);
  assert(wcs);
  assert(wcs_length > 0);

  int utf8_length = WideCharToMultiByte(CP_UTF8, 0, wcs, wcs_length, NULL, 0, NULL, NULL);
  osutil_string_reserve(string, utf8_length + 1);
  string->size = WideCharToMultiByte(CP_UTF8, 0, wcs, wcs_length, string->data, string->capacity, NULL, NULL);
  string->data[string->size] = '\0';
}
