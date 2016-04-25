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
add_subdirectory(src/tests)
add_subdirectory(submodules/unittest-cpp)

if (WIN32)
  string(REPLACE "/W3" "/W0" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
endif()

## libuv
file(GLOB libuvfiles
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/*.h
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/win/*.c
    ${CMAKE_SOURCE_DIR}/submodules/libuv/src/win/*.h
)

add_library(libuv STATIC ${libuvfiles})
