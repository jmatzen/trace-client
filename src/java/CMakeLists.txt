file(GLOB files *.cpp *.h ../../include/trace/*.h)

add_library(trace-java SHARED ${files})
include_directories (C:/java/jdk1.8.0_45/include C:/java/jdk1.8.0_45/include/win32)

if (WIN32)
  target_link_libraries(trace-java libuv ws2_32 userenv Iphlpapi psapi)
endif()

