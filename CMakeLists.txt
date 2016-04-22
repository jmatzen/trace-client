cmake_minimum_required (VERSION 3.0)
project (trace-client)

include_directories (${CMAKE_SOURCE_DIR}/include)

if (APPLE)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)
endif()


file(GLOB files src/trace/*.cpp src/trace/*.h include/trace/*.h)
add_library(ayxiatrace SHARED ${files})
if (WIN32)
  target_link_libraries(ayxiatrace ws2_32)
endif()


file(GLOB files src/sample_c/*.c src/sample_c/*.h)
add_executable(sample_c ${files})
target_link_libraries(sample_c ayxiatrace)

file(GLOB files src/sample_cpp/*.cpp src/sample_cpp/*.h)
add_executable(sample_cpp ${files})
target_link_libraries(sample_cpp ayxiatrace)
