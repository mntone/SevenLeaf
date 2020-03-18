function(add_filepath_macro target)
  get_target_property(SOURCE_FILES ${target} SOURCES)

  foreach (FILE_PATH IN LISTS SOURCE_FILES)
    file(RELATIVE_PATH RELATIVE_FILE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH})
    set_property(SOURCE ${FILE_PATH} APPEND PROPERTY COMPILE_DEFINITIONS __FILEPATH__="${RELATIVE_FILE_PATH}")
  endforeach()
endfunction()
