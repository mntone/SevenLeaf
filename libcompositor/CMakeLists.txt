cmake_minimum_required(VERSION 3.9)

project(libcompositor)

# Add deps
if(WIN32)
  include_directories(libcompositor
    "$ENV{DXSDK_DIR}/Include/${DIRECTX_ARCH}/um"
  )

  set(libcompositor_DEPS
    dxguid.lib
    dxgi.lib
    d3d12.lib
    d3dcompiler.lib
  )
elseif(APPLE)
  find_library(COREGRAPHICS CoreGraphics)
  find_library(IOSURFACE IOSurface)
  find_library(METAL Metal)
  find_library(QUARTZCORE QuartzCore)
  include_directories(${COREGRAPHICS} ${IOSURFACE} ${METAL} ${QUARTZCORE})

  set(libcompositor_DEPS
    ${COREGRAPHICS}
    ${IOSURFACE}
    ${METAL}
    ${QUARTZCORE}
  )
endif()

# Add vector files
set(libcompositor_vector_HEADERS
  include/compositor/vector/simd_base.h
  include/compositor/vector/int16x8_t.h
  include/compositor/vector/uint16x8_t.h
  include/compositor/vector/float32x2_t.h
  include/compositor/vector/float32x4_t.h
  include/compositor/vector/float32x4x2_t.h
  include/compositor/vector/matrix3x2_t.h
  include/compositor/vector/matrix4x4_t.h
  include/compositor/vector/CpsrSingle4x4.h
)
set(libcompositor_vector_SOURCES
  source/vector/CpsrVector.cpp
)
source_group("vector\\Header Files" FILES ${libcompositor_vector_HEADERS})
source_group("vector\\Source Files" FILES ${libcompositor_vector_SOURCES})

# Add root files
set(libcompositor_HEADERS
  include/compositor/CpsrTypedefs.h
  include/compositor/CpsrUtils.h
  include/compositor/CpsrGraphics.h
  include/compositor/CpsrGraphics.hpp
  ${libcompositor_vector_HEADERS}
)
set(libcompositor_SOURCES
  ${libcompositor_vector_SOURCES}
)

if(APPLE)
  list(APPEND libcompositor_SOURCES
    source/mac/CpsrCpu.m
  )
endif()

if(WIN32)
  list(APPEND libcompositor_HEADERS
    source/dx12/CpsrGraphics+Private.h
  )
  list(APPEND libcompositor_SOURCES
    source/dx12/CpsrDevice.cpp
    source/dx12/CpsrFence.c
    source/dx12/CpsrHeap.c
    source/dx12/CpsrBuffer.c
    source/dx12/CpsrTexture2D.c
    source/dx12/CpsrSwapChain.c
    source/dx12/CpsrCommandQueue.c
    source/dx12/CpsrCommandBuffer.c
    source/dx12/CpsrGraphicsPipelineState.c
    source/dx12/CpsrGraphicsContext.c
    source/dx12/CpsrComputePipelineState.c
    source/dx12/CpsrComputeContext.c
    source/dx12/CpsrShaderLibrary.c
    source/dx12/CpsrShaderFunction.c
  )
elseif(APPLE)
  list(APPEND libcompositor_HEADERS
    source/mtl2/CpsrGraphics+Private.h
  )
  list(APPEND libcompositor_SOURCES
    source/mtl2/CpsrDevice.m
    source/mtl2/CpsrFence.m
    source/mtl2/CpsrHeap.m
    source/mtl2/CpsrBuffer.m
    source/mtl2/CpsrTexture2D.m
    source/mtl2/CpsrSwapChain.m
    source/mtl2/CpsrCommandQueue.m
    source/mtl2/CpsrCommandBuffer.m
    source/mtl2/CpsrGraphicsPipelineState.m
    source/mtl2/CpsrGraphicsContext.m
    source/mtl2/CpsrComputePipelineState.m
    source/mtl2/CpsrComputeContext.m
    source/mtl2/CpsrShaderLibrary.m
    source/mtl2/CpsrShaderFunction.m
  )
endif()

include_directories(libcompositor include)
add_library(libcompositor SHARED
  ${libcompositor_HEADERS}
  ${libcompositor_SOURCES}
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
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      add_linker_flags(-Wl,--large-address-aware)
    endif()
  elseif(MSVC)
    add_compiler_flags(/fp:fast)
    add_linker_flags(/STACK:131072)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      add_linker_flags(/LARGEADDRESSAWARE)
    endif()
  endif()
else()
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    add_compile_options(/fp:fast)
  endif()
endif()

if(NOT MSVC)
  target_compile_options(libcompositor PUBLIC -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2)
endif()

add_filepath_macro(libcompositor)
set_target_properties(libcompositor PROPERTIES OUTPUT_NAME compositor)
target_link_libraries(libcompositor PRIVATE ${libcompositor_DEPS})
snlf_install_library(libcompositor)
