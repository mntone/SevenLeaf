#include "osutil_filepath.h"

#include <mach-o/dyld.h>

#import <Foundation/Foundation.h>

osutil_string_t osutil_getpath_current() {
  NSString *ns_current = [NSFileManager defaultManager].currentDirectoryPath;
  
  osutil_string_t ret = { 0 };
  osutil_string_copy_with_cfstring(&ret, (CFStringRef)ns_current);
  [ns_current release];
  return ret;
}

osutil_string_t osutil_getpath_executable() {
  uint32_t length = 0;
  _NSGetExecutablePath(NULL, &length);
  
  osutil_string_t ret = { 0 };
  const uint32_t max_size = length + 1;
  if (osutil_string_reserve(&ret, max_size) < max_size) {
    return ret;
  }
  length = ret.capacity;
  if (_NSGetExecutablePath(ret.data, &length)) {
    return ret;
  }
  
  ret.size = length;
  return ret;
}

static inline osutil_string_t osutil_getpath_first(NSSearchPathDirectory directory, NSSearchPathDomainMask domainMask) {
  osutil_string_t ret = { 0 };
  
  NSArray<NSString *> *pathes = NSSearchPathForDirectoriesInDomains(directory, domainMask, YES);
  if (pathes.count == 0) {
    [pathes release];
    return ret;
  }
  
  osutil_string_copy_with_cfstring(&ret, (CFStringRef)pathes[0]);
  
  for (size_t i = 0; i < pathes.count; ++i) {
    [pathes[i] release];
  }
  [pathes release];
  return ret;
}

osutil_string_t osutil_getpath_document() {
  return osutil_getpath_first(NSDocumentDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_photo() {
  return osutil_getpath_first(NSPicturesDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_screenshots() {
  return osutil_getpath_first(NSPicturesDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_music() {
  return osutil_getpath_first(NSMusicDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_video() {
  return osutil_getpath_first(NSMoviesDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_download() {
  return osutil_getpath_first(NSDownloadsDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_program_data() {
  return osutil_getpath_first(NSApplicationSupportDirectory, NSLocalDomainMask);
}

osutil_string_t osutil_getpath_config() {
  return osutil_getpath_first(NSApplicationSupportDirectory, NSUserDomainMask);
  
  // TODO: iOS exist -> create directory
}

osutil_string_t osutil_getpath_cache() {
  return osutil_getpath_first(NSCachesDirectory, NSUserDomainMask);
}

osutil_string_t osutil_getpath_temporary() {
  NSString *temporary = NSTemporaryDirectory();
  
  osutil_string_t ret = { 0 };
  osutil_string_copy_with_cfstring(&ret, (CFStringRef)temporary);
  [temporary release];
  return ret;
}
