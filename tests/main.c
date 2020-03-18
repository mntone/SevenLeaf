#include <stdio.h>

#include <osutil.h>

int main(int argc, char *argv[]) {
  //CpsrEnumGraphicsDevice(d);
  
  osutil_string_t cur = osutil_getpath_current();
  osutil_string_t exe = osutil_getpath_executable();
  osutil_string_t doc = osutil_getpath_document();
  osutil_string_t photo = osutil_getpath_photo();
  osutil_string_t ss = osutil_getpath_screenshots();
  osutil_string_t music = osutil_getpath_music();
  osutil_string_t video = osutil_getpath_video();
  osutil_string_t download = osutil_getpath_download();
  osutil_string_t progdata = osutil_getpath_program_data();
  osutil_string_t config = osutil_getpath_config();
  osutil_string_t cache = osutil_getpath_cache();
  osutil_string_t temp = osutil_getpath_temporary();
  
  osutil_string_free(&cur);
  osutil_string_free(&exe);
  osutil_string_free(&doc);
  osutil_string_free(&photo);
  osutil_string_free(&ss);
  osutil_string_free(&music);
  osutil_string_free(&video);
  osutil_string_free(&download);
  osutil_string_free(&progdata);
  osutil_string_free(&config);
  osutil_string_free(&cache);
  osutil_string_free(&temp);
  return 0;
}
