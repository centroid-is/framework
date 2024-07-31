function(tfc_split_debug_info target)
  if (UNIX AND ENABLE_DEBUG_SYMBOLS_IN_RELEASE)
    if(NOT CMAKE_STRIP OR NOT CMAKE_OBJCOPY)
      message(FATAL_ERROR "missing CMAKE_STRIP or CMAKE_OBJCOPY")
    endif()
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "EXECUTABLE")
      add_custom_command(TARGET ${target} POST_BUILD
          COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.sym
          COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${target}>
          COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${target}>.sym $<TARGET_FILE:${target}>
      )
    else()
      message(FATAL_ERROR "Target ${target} is not valid for symbols splitting, must be shared library or executable")
    endif()
  endif()
endfunction()
