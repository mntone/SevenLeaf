cmake_minimum_required(VERSION 3.9)

project(generic_utils)

# Add deps
find_package(wxWidgets ${wxWidgets_VERSION} COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(generic_utils
  "${CMAKE_SOURCE_DIR}/libosutil/include"
  "${CMAKE_SOURCE_DIR}/libcompositor/include"
  "${CMAKE_SOURCE_DIR}/libsevenleaf/include"
)

set(generic_utils_DEPS
  libosutil
  libcompositor
  libsevenleaf
  ${wxWidgets_LIBRARIES}
)

# Add files
set(generic_utils_SOURCES
  source/SnlfColorInput.c
)

if(APPLE)
  set(generic_utils_RESOURCES
    source/apple/ColorInputShader.metal
  )
  set_source_files_properties(${generic_utils_RESOURCES} PROPERTIES LANGUAGE METAL)
  source_group("Resource Files" FILES ${generic_utils_RESOURCES})
endif()

add_library(generic_utils SHARED
  ${generic_utils_RESOURCES}
  ${generic_utils_SOURCES}
)

# Build config
add_filepath_macro(generic_utils)
set_target_properties(generic_utils PROPERTIES OUTPUT_NAME generic)
target_link_libraries(generic_utils PRIVATE ${generic_utils_DEPS})
snlf_install_plugin(generic_utils TRUE)
