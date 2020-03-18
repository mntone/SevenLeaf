cmake_minimum_required(VERSION 3.9)

project(apple_avcapture)

# Add deps
find_library(AVFOUNDATION AVFoundation)
find_library(COREMEDIA CoreMedia)
find_library(COREVIDEO CoreVideo)
include_directories(${AVFOUNDATION} ${COREMEDIA} ${COREVIDEO})

find_package(wxWidgets ${wxWidgets_VERSION} COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(apple_avcapture
  "${CMAKE_SOURCE_DIR}/libosutil/include"
  "${CMAKE_SOURCE_DIR}/libcompositor/include"
  "${CMAKE_SOURCE_DIR}/libsevenleaf/include"
)

set(apple_avcapture_DEPS
  libosutil
  libcompositor
  libsevenleaf
  ${AVFOUNDATION}
  ${COREMEDIA}
  ${COREVIDEO}
  ${wxWidgets_LIBRARIES}
)

# Add files
set(apple_avcapture_HEADERS
  source/SnlfAVDevicesObserver.h
)
set(apple_avcapture_SOURCES
  source/SnlfAVDevicesObserver.m
  source/SnlfAVCapture.m
)

add_library(apple_avcapture SHARED
  ${apple_avcapture_HEADERS}
  ${apple_avcapture_SOURCES}
)

# Build config
add_filepath_macro(apple_avcapture)
set_target_properties(apple_avcapture PROPERTIES OUTPUT_NAME aplavcap)
target_link_libraries(apple_avcapture PRIVATE ${apple_avcapture_DEPS})
snlf_install_plugin(apple_avcapture)
