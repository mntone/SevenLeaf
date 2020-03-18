cmake_minimum_required(VERSION 2.8)

option(SNLF_DISABLE_PLUGINS "Disable plugins build" OFF)
if(SNLF_DISABLE_PLUGINS)
  message(STATUS "SNLF_DISABLE_PLUGINS is set; plugins build is disabled.")
  return()
endif()

add_subdirectory(generic-utils)

if(WIN32)
elseif(APPLE)
  #add_subdirectory(apple-coregraphics)
  add_subdirectory(apple-avcapture)
endif()
