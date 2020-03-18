cmake_minimum_required(VERSION 3.1)

project(snlftest)

# Add deps
include_directories(snlftest
  "${CMAKE_SOURCE_DIR}/libosutil/include"
  "${CMAKE_SOURCE_DIR}/libcompositor/include")
set(snlftest_DEPS
  libosutil
  libcompositor
  #libsevenleaf
)

# Add files
set(snlftest_SOURCES
  main.c
)

add_executable(snlftest ${snlftest_SOURCES})

# Build config
set_target_properties(snlftest PROPERTIES OUTPUT_NAME snlftest)
target_link_libraries(snlftest PRIVATE ${snlftest_DEPS})
