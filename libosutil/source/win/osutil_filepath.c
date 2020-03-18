#include "osutil_filepath.h"

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ShlObj.h>

osutil_string_t osutil_getpath_current() {
  osutil_string_t ret = {0};

  wchar_t buf[MAX_PATH + 1];
  DWORD length = GetCurrentDirectoryW(MAX_PATH + 1, buf);
  if (length < 0) {
    return ret;
  }

  osutil_string_copy_from_wcstring_and_length(&ret, buf, length);
  return ret;
}

osutil_string_t osutil_getpath_executable() {
  osutil_string_t ret = {0};

  wchar_t buf[MAX_PATH + 1];
  DWORD length = GetModuleFileNameW(NULL, buf, MAX_PATH + 1);
  if (length < 0) {
    return ret;
  }

  osutil_string_copy_from_wcstring_and_length(&ret, buf, length);
  return ret;
}

static inline osutil_string_t osutil_getpath_first(const KNOWNFOLDERID *folderId) {
  osutil_string_t ret = {0};

  PWSTR w_path;
  HRESULT hr = SHGetKnownFolderPath(folderId, KF_FLAG_CREATE, NULL, &w_path);
  if (FAILED(hr)) {
    return ret;
  }

  osutil_string_copy_from_wcstring(&ret, w_path);
  CoTaskMemFree(w_path);
  return ret;
}

osutil_string_t osutil_getpath_document() {
  const GUID folderId = FOLDERID_Documents;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_photo() {
  const GUID folderId = FOLDERID_Pictures;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_screenshots() {
  const GUID folderId = FOLDERID_Screenshots;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_music() {
  const GUID folderId = FOLDERID_Music;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_video() {
  const GUID folderId = FOLDERID_Videos;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_download() {
  const GUID folderId = FOLDERID_Downloads;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_program_data() {
  const GUID folderId = FOLDERID_ProgramData;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_config() {
  const GUID folderId = FOLDERID_RoamingAppData;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_cache() {
  const GUID folderId = FOLDERID_LocalAppData;
  return osutil_getpath_first(&folderId);
}

osutil_string_t osutil_getpath_temporary() {
  osutil_string_t ret = {0};

  wchar_t buf[MAX_PATH + 1];
  DWORD length = GetTempPathW(MAX_PATH + 1, buf);
  if (length < 0) {
    return ret;
  }

  osutil_string_copy_from_wcstring_and_length(&ret, buf, length);
  return ret;
}
