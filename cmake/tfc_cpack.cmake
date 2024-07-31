
function(priv_get_directory DIRECTORY)
  set(${DIRECTORY} ${CMAKE_CURRENT_FUNCTION_LIST_DIR} PARENT_SCOPE)
endfunction()

macro(tfc_cpack_init PACKAGE_NAME LICENSE_PATH README_PATH)
  set (CPACK_DEB_COMPONENT_INSTALL ON)
  set (CPACK_RPM_COMPONENT_INSTALL ON)
  set (CPACK_ARCHIVE_COMPONENT_INSTALL ON)

  set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

  # get glibc version from host
  execute_process(
      COMMAND bash -c "ldd --version | awk '/ldd/{print $NF;exit}'"
      OUTPUT_VARIABLE LIBC_VERSION
      RESULT_VARIABLE LDD_RESULT
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  set(CPACK_PACKAGE_NAME ${PACKAGE_NAME})
  set(CPACK_PACKAGE_VENDOR Centroid)
  set(CPACK_PACKAGE_CONTACT "Centroid <centroid@centroid.is>")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
  #set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME}) # todo
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  # professional cmake suggests this to be explicitly set to true because it defaults to FALSE for backwards compatibility.
  set(CPACK_VERBATIM_VARIABLES YES)

  #set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Installation prefix" FORCE)
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= ${LIBC_VERSION}), python3")
  set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= ${LIBC_VERSION}, python3")

  if(CMAKE_STRIP)
    set(CPACK_RPM_SPEC_MORE_DEFINE "%define __strip ${CMAKE_STRIP}")
  endif()

  if (${CMAKE_SYSTEM_PROCESSOR} MATCHES aarch64)
    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-arm64")  # todo correct?
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE arm64)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE aarch64)
  else ()
    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
    set(CPACK_RPM_PACKAGE_ARCHITECTURE x86_64)
  endif ()

  priv_get_directory(THIS_DIRECTORY)
  SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${THIS_DIRECTORY}/scripts/postinst")
  set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${THIS_DIRECTORY}/scripts/postinst")

  # TODO graphical installer properties
  #set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_LIST_DIR}/Description.txt)
  #set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_CURRENT_LIST_DIR}/Welcome.txt)
  set(CPACK_RESOURCE_FILE_LICENSE ${LICENSE_PATH})
  set(CPACK_RESOURCE_FILE_README ${README_PATH})

  include(CPack)
endmacro()

function(tfc_add_component_group TARGET_NAME GROUP_NAME DESCRIPTION)
  # Determine component name and display name based on build type
  set(COMPONENT_NAME ${GROUP_NAME})
  set(DISPLAY_NAME ${COMPONENT_NAME})
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(COMPONENT_NAME "${COMPONENT_NAME}-dbg")
    set(DISPLAY_NAME "${TARGET_NAME} (Debug)")
  endif ()

  # Install the target
  install(
    TARGETS ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${COMPONENT_NAME}
  )

  # Add the component to CPack
  cpack_add_component(
    ${COMPONENT_NAME}
    DISPLAY_NAME ${DISPLAY_NAME}
    DESCRIPTION ${DESCRIPTION}
    GROUP ${COMPONENT_NAME}
    INSTALL_TYPES Full
  )
endfunction()

function(tfc_add_component TARGET_NAME DESCRIPTION)
  tfc_add_component_group(${TARGET_NAME} ${TARGET_NAME} ${DESCRIPTION})
endfunction()
