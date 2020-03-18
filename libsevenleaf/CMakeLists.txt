cmake_minimum_required(VERSION 3.9)

project(libsevenleaf)

# Add deps
find_package(wxWidgets ${wxWidgets_VERSION} COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(libsevenleaf
  "${CMAKE_SOURCE_DIR}/libosutil/include"
  "${CMAKE_SOURCE_DIR}/libcompositor/include"
)

set(libsevenleaf_DEPS
  libosutil
  libcompositor
  ${wxWidgets_LIBRARIES}
)
if(APPLE)
  find_library(APPKIT AppKit)
  include_directories(${APPKIT})

  list(APPEND libsevenleaf_DEPS
    ${APPKIT}
  )
endif()

# Add containers files
set(libsevenleaf_containers_HEADERS
  include/containers/SnlfArray.h
  include/containers/SnlfLockFreeQueue+Prototypes.h
  include/containers/SnlfLockFreeQueue.h
)
source_group("containers\\Header Files" FILES ${libsevenleaf_containers_HEADERS})

# Add geometry files
set(sevenleafapp_geometry_HEADERS
  include/geometry/SnlfCircleGeometry.hpp
  include/geometry/SnlfPlaneGeometry.hpp
  include/geometry/SnlfRectangleGeometry.hpp
)
source_group("geometry\\Header Files" FILES ${sevenleafapp_geometry_HEADERS})

# Add root files
set(libsevenleaf_HEADERS
  include/SnlfConfig.h
  include/SnlfLog.h
  include/SnlfGraphicsDefines.h
  include/SnlfGraphicsFrame.h
  include/SnlfCore.h
  include/SnlfPrimitives.h
  include/SnlfDisplay.h
  include/SnlfDisplayWindow.hpp
  include/SnlfModule.h
  include/utils/SnlfMath.hpp
  ${libsevenleaf_containers_HEADERS}
  ${sevenleafapp_geometry_HEADERS}

  source/SnlfMessage.h
  source/SnlfCore+Private.h
  source/SnlfGraphics+Private.h
  source/SnlfGraphicsFrame+Private.h
  source/SnlfUtils+Private.h
)
set(libsevenleaf_SHARED_SOURCES
  source/SnlfLog.c
  source/SnlfCore.c
  source/SnlfModule.c
  source/SnlfMessage.c
  source/SnlfObject.c
  source/SnlfGenerator.c
  source/SnlfGeneratorSourceGraphics.c
  source/SnlfInput.c
  source/SnlfInputSourceGraphics.c
  source/SnlfSource.c
  source/SnlfSourceGraphics.c
  source/SnlfGraphics.c
  source/SnlfGraphicsContext.c
  source/SnlfGraphicsFrame.c
  source/SnlfTransition.c
  source/SnlfTransitionGraphics.c
  source/SnlfOutput.c
  source/SnlfDisplay.c
  source/SnlfDisplayWindow.cpp
)
if(WIN32)
  set(libsevenleaf_SOURCES
    ${libsevenleaf_SHARED_SOURCES}
  )
elseif(APPLE)
  set(libsevenleaf_RESOURCES
    source/apple/Draw.metal
    source/apple/DrawColor.metal
    source/apple/Posterization.metal
  )
  set_source_files_properties(${libsevenleaf_RESOURCES} PROPERTIES LANGUAGE METAL)
  source_group("Resource Files" FILES ${libsevenleaf_RESOURCES})

  set(libsevenleaf_SOURCES
    ${libsevenleaf_SHARED_SOURCES}
    source/apple/SnlfModule-apple.m
  )
endif()

include_directories(libsevenleaf include)
add_library(libsevenleaf SHARED
  ${libsevenleaf_RESOURCES}
  ${libsevenleaf_HEADERS}
  ${libsevenleaf_SOURCES}
)

# Build config
add_filepath_macro(libsevenleaf)
set_target_properties(libsevenleaf PROPERTIES OUTPUT_NAME sevenleaf)
target_link_libraries(libsevenleaf PRIVATE ${libsevenleaf_DEPS})
snlf_install_library(libsevenleaf TRUE)
