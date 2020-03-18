cmake_minimum_required(VERSION 3.9)

project(libosutil)

# Add root files
set(libosutil_HEADERS
  include/osutil.h
  include/osutil_base.h
  include/osutil_system.h
  include/osutil_time.h
  include/osutil_thread.h
  include/osutil_file.h
  include/osutil_filepath.h
  include/osutil_string.h
  include/osutil_dll.h
  include/osutil_uint128_t.h
)
if(WIN32)
  list(APPEND libosutil_HEADERS
    include/osutil_atomic_win.h
  )
else()
  list(APPEND libosutil_HEADERS
    include/osutil_atomic_posix.h
  )
endif()

set(libosutil_SHARED_SOURCES
)
if(WIN32)
  set(libosutil_SOURCES
    source/win/osutil_system.c
    source/win/osutil_time.c
    source/win/osutil_thread.c
    source/win/osutil_file.c
    source/win/osutil_filepath.c
    source/win/osutil_string.c
    source/win/osutil_dll.c
    ${libosutil_SHARED_SOURCES}
  )
elseif(APPLE)
  set(libosutil_SOURCES
    source/apple/osutil_system.c
    source/apple/osutil_time.c
    source/apple/osutil_thread.m
    source/posix/osutil_file.c
    source/apple/osutil_filepath.m
    source/posix/osutil_string.c
    source/posix/osutil_dll.c
    ${libosutil_SHARED_SOURCES}
  )
else()
  set(libosutil_SOURCES
    source/linux/osutil_time.c
    source/posix/osutil_string.c
    source/posix/osutil_dll.c
    ${libosutil_SHARED_SOURCES}
  )
endif()

include_directories(libosutil include)
add_library(libosutil SHARED
  ${libosutil_HEADERS}
  ${libosutil_SOURCES}
)

# Build config
macro(add_compiler_flags)
  string (REPLACE ";" " " _FLAGS "${ARGV}")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS}")
endmacro()

macro(add_linker_flags)
  string(REPLACE ";" " " _FLAGS "${ARGV}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${_FLAGS}")
endmacro()

if(WIN32)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    add_compiler_flags(-ffast-math)
    add_linker_flags(-Wl,--stack,131072)
  elseif(MSVC)
    add_compiler_flags(/fp:fast)
    add_linker_flags(/STACK:131072)
  endif()
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    add_compile_options(/fp:fast)
  endif()
endif()

add_filepath_macro(libosutil)
set_target_properties(libosutil PROPERTIES OUTPUT_NAME osutil)
snlf_install_library(libosutil)
