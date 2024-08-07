project(dbus_util)

add_library(dbus_util src/dbus_util.cpp src/exception.cpp src/sml_interface.cpp src/string_maker.cpp)
add_library(tfc::dbus_util ALIAS dbus_util)
target_include_directories(dbus_util
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inc/public>
    $<INSTALL_INTERFACE:include>
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDBUSPLUS REQUIRED IMPORTED_TARGET GLOBAL sdbusplus)
find_package(magic_enum CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(dbus_util
  PUBLIC
    PkgConfig::SDBUSPLUS
    magic_enum::magic_enum
    tfc::stx
    tfc::configure_options
    tfc::logger
    fmt::fmt
  PRIVATE
    tfc::base
)

add_library_to_docs(tfc::dbus_util)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()

include(tfc_install)
tfc_install_lib(dbus_util)
