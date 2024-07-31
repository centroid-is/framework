set(CMAKE_SYSTEM_PROCESSOR x64)
set(COMPILER_MINIMUM_VERSION 17)

if (EXISTS "/cpproot/bin/clang")
#  TODO compile libc in container
#  set(CMAKE_SYSROOT /cpproot)
  set(CMAKE_C_COMPILER /cpproot/bin/clang)
  set(CMAKE_CXX_COMPILER /cpproot/bin/clang++)
  set(CMAKE_CXX_FLAGS "-stdlib=libc++ -I/cpproot/include/ -I/cpproot/include/x86_64-unknown-linux-gnu/c++/v1")
  set(CMAKE_C_FLAGS "-I/cpproot/include/ -I/cpproot/include/x86_64-unknown-linux-gnu/c++/v1")

  set(ENV{CC} /cpproot/bin/clang)
  set(ENV{CXX} /cpproot/bin/clang++)
  set(ENV{CXXFLAGS} "-stdlib=libc++ -I/cpproot/include/ -I/cpproot/include/x86_64-unknown-linux-gnu/c++/v1")
  set(ENV{CFLAGS} "-I/cpproot/include/ -I/cpproot/include/x86_64-unknown-linux-gnu/c++/v1")

  # Specify search paths for libraries
  set(ENV{LIBRARY_PATH} "/cpproot/lib:/cpproot/lib/x86_64-unknown-linux-gnu:$ENV{LIBRARY_PATH}")
else ()
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
  set(ENV{CC} clang)
  set(ENV{CXX} clang++)
endif ()
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld -stdlib=libc++")
set(ENV{LDFLAGS} "-fuse-ld=lld -stdlib=libc++")

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE COMPILER_VERSION)

if (COMPILER_VERSION STREQUAL "")
  message(FATAL_ERROR "Could not determine Clang version!")
endif ()

if (COMPILER_VERSION VERSION_LESS COMPILER_MINIMUM_VERSION)
  message(FATAL_ERROR "Clang version must be at least ${COMPILER_MINIMUM_VERSION}! Found ${COMPILER_VERSION}")
endif ()
