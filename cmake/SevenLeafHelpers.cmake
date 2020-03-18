set(SNLF_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output")

if(UNIX AND NOT APPLE)
  set(SNLF_EXECUTABLE_DESTINATION "bin")
  set(SNLF_LIBRARY_DESTINATION    "lib")
  set(SNLF_PLUGINS_DESTINATION    "plg")
  set(SNLF_RESOURCES_DESTINATION  "res")
elseif(APPLE)
  set(SNLF_EXECUTABLE_DESTINATION "MacOS")
  set(SNLF_LIBRARY_DESTINATION    "Frameworks")
  set(SNLF_PLUGINS_DESTINATION    "Plugins")
  set(SNLF_RESOURCES_DESTINATION  "Resources")
else()
  set(SNLF_EXECUTABLE_DESTINATION "bin")
  set(SNLF_LIBRARY_DESTINATION    "bin")
  set(SNLF_PLUGINS_DESTINATION    "plg")
  set(SNLF_RESOURCES_DESTINATION  "res")
endif()

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SNLF_OUTPUT_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SNLF_OUTPUT_DIR})
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SNLF_OUTPUT_DIR})

#function(snlf_install_executable target)
#  get_filename_component(BUNDLE_PATH $<TARGET_FILE_DIR:${target}> PATH)
#  get_filename_component(BUNDLE_NAME $<TARGET_FILE_DIR:${target}> NAME)
#  add_custom_command(TARGET ${target} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy "${BUNDLE_PATH}/${BUNDLE_NAME}"
#      "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>/${BUNDLE_NAME}"
#      VERBATIM)
#endfunction()

function(snlf_install_library target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}>"
      "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>/${SNLF_LIBRARY_DESTINATION}/$<TARGET_FILE_NAME:${target}>"
      VERBATIM)
  if(${ARGV1})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${target}>/default.metallib"
        "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>/${SNLF_RESOURCES_DESTINATION}/default.metallib"
        VERBATIM)
  endif()
endfunction()

function(snlf_install_plugin target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}>"
      "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>/${SNLF_PLUGINS_DESTINATION}/$<TARGET_LINKER_FILE_PREFIX:${target}>$<TARGET_LINKER_FILE_BASE_NAME:${target}>/$<TARGET_FILE_NAME:${target}>"
      VERBATIM)
    if(${ARGV1})
      add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${target}>/default.metallib"
          "${SNLF_OUTPUT_DIR}/$<CONFIGURATION>/${SNLF_PLUGINS_DESTINATION}/$<TARGET_LINKER_FILE_PREFIX:${target}>$<TARGET_LINKER_FILE_BASE_NAME:${target}>/default.metallib"
          VERBATIM)
    endif()
endfunction()
