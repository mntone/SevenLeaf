#include "osutil_thread.h"

#include <string.h>

#import <Foundation/Foundation.h>

int osutil_os_version(char *os_version, size_t len) {
  size_t ret = 0;
  NSString *ns_os_version = [NSProcessInfo processInfo].operatingSystemVersionString;
  if (os_version) {
    ret = strlcpy(os_version, ns_os_version.UTF8String, len);
  } else {
    ret = strlen(ns_os_version.UTF8String);
  }
  [ns_os_version release];
  return ret;
}

process_identifier_t osutil_get_process_identifier() {
  return [NSProcessInfo processInfo].processIdentifier;
}

thread_identifier_t osutil_get_thread_identifier() {
  return [NSThread currentThread];
}

void osutil_set_thread_name(const char *thread_name) {
  NSString *ns_thread_name = [[NSString alloc] initWithBytesNoCopy:(void *)thread_name length:strlen(thread_name) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  [[NSThread currentThread] setName:ns_thread_name];
  [ns_thread_name release];
}
