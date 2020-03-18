cmake_minimum_required(VERSION 3.9)

if(NOT CMAKE_CONFIGURATION_TYPES)
  get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(HAVE_MULTI_CONFIG_GENERATOR)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
  endif()
endif()

# Add project settings
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT MSVC)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

if(APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)
  list(APPEND CMAKE_INSTALL_RPATH "@executable_path/" "@executable_path/../Frameworks")

  set(CMAKE_XCODE_ATTRIBUTE_GCC_C_LANGUAGE_STANDARD     "c11")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY           "libc++")
else()
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Include projects
project(SevenLeaf)

set(wxWidgets_VERSION "3.1")

include(AddFilepathMacro)
include(SevenLeafHelpers)

add_subdirectory(deps)

add_subdirectory(libosutil)
add_subdirectory(libcompositor)
add_subdirectory(libsevenleaf)
add_subdirectory(sevenleafapp)

add_subdirectory(plugins)
add_subdirectory(tests)
