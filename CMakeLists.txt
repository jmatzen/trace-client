cmake_minimum_required (VERSION 3.0)
project (trace-client)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories (${CMAKE_SOURCE_DIR}/submodules/libuv/include)

if (APPLE)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)
endif()

if (WIN32)
  add_definitions(/MP)
endif()

add_subdirectory(src/trace)
add_subdirectory(src/sample_c)
add_subdirectory(src/sample_cpp)

if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
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
