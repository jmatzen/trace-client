cmake_minimum_required (VERSION 3.0)
project (trace-client)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories (${CMAKE_SOURCE_DIR}/submodules/libuv/include)
include_directories (${CMAKE_SOURCE_DIR}/submodules/unittest-cpp)

if (APPLE)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)
endif()

if (WIN32)
  add_definitions(/MP)
  add_definitions(/D_WIN32_WINNT=0x0600 /DWINVER=0x0600)
  add_definitions(/D_SCL_SECURE_NO_WARNINGS /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS)

endif()

add_subdirectory(src/trace)
add_subdirectory(src/sample_c)
add_subdirectory(src/sample_cpp)
#add_subdirectory(src/tests)
add_subdirectory(submodules/unittest-cpp)

if (WIN32)
  string(REPLACE "/W3" "/W0" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
endif()

if (WIN32)
## libuv
file(GLOB libuvfiles
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/win/*.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/win/*.h
)
elseif(APPLE)
  file(GLOB libuvfiles
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/darwin.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/fsevents.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/darwin-proctitle.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/proctitle.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/async.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/atomic-ops.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/core.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/dl.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/fs.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/getaddrinfo.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/getnameinfo.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/internal.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/loop.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/loop-watcher.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/pipe.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/poll.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/process.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/signal.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/spinlock.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/stream.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/tcp.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/thread.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/timer.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/tty.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/udp.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/unix/kqueue.c
   
  )
  include_directories(${CMAKE_SOURCE_DIR}/submodules/libuv/src)
  add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
endif()
add_library(libuv STATIC ${libuvfiles})

#include_external_msproject(
#  TestAyxiaDotNet ${CMAKE_SOURCE_DIR}/src/TestAyxiaDotNet/TestAyxiaDotNet.csproj
#  TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)

#include_external_msproject(
#  ayxiadotnet ${CMAKE_SOURCE_DIR}/src/ayxiadotnet/ayxiadotnet.csproj
#  TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)
