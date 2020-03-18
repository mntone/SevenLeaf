cmake_minimum_required(VERSION 3.9)

project(sevenleafapp)

# Add deps
find_package(wxWidgets ${wxWidgets_VERSION} COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(sevenleafapp
  "${CMAKE_SOURCE_DIR}/libosutil/include"
  "${CMAKE_SOURCE_DIR}/libcompositor/include"
  "${CMAKE_SOURCE_DIR}/libsevenleaf/include"
)

set(sevenleafapp_DEPS
  libosutil
  libcompositor
  libsevenleaf
  ${wxWidgets_LIBRARIES}
)

# Add files
set(sevenleafapp_HEADERS
  include/SnlfApp.hpp
  include/SnlfPreviewWindow.hpp
  ${sevenleafapp_primitives_HEADERS}
)
set(sevenleafapp_SOURCES
  source/SnlfApp.cpp
  source/SnlfPreviewWindow.cpp
)

include_directories(sevenleafapp include)
add_executable(sevenleafapp WIN32 MACOSX_BUNDLE
  ${sevenleafapp_HEADERS}
  ${sevenleafapp_SOURCES}
)

# Build config
add_filepath_macro(sevenleafapp)
if(APPLE)
  set_target_properties(sevenleafapp PROPERTIES
    XCODE_SCHEME_NAME        "sevenleafapp|Debug"
    XCODE_SCHEME_LAUNCH_OPTS "enableGPUFrameCaptureMode=1"
  )
endif()

if(APPLE)
  set_target_properties(sevenleafapp PROPERTIES
    OUTPUT_NAME              "SevenLeaf"
    APP_IDENTIFIER           "org.mntone.SevenLeaf"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/Info.plist.in"
    RUNTIME_OUTPUT_DIRECTORY "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>"
  )
else()
  set_target_properties(sevenleafapp PROPERTIES
    OUTPUT_NAME snlfapp
  )
endif()

target_link_libraries(sevenleafapp PRIVATE ${sevenleafapp_DEPS})
#snlf_install_executable(sevenleafapp)
