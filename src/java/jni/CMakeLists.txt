file(GLOB files *.c *.h ../../include/trace/*.h)

add_library(trace-java SHARED ${files})
include_directories (C:/java/jdk1.8.0_92/include C:/java/jdk1.8.0_92/include/win32)
add_definitions(/std:c++latest)

if (WIN32)
  target_link_libraries(trace-java libuv ws2_32 userenv Iphlpapi psapi ayxiatrace)
endif()

