file(GLOB files *.cpp *.h ../../include/trace/*.h)

add_library(ayxiatrace SHARED ${files})

if (WIN32)
  target_link_libraries(ayxiatrace libuv ws2_32 userenv Iphlpapi psapi)
elseif(APPLE)
  target_link_libraries(ayxiatrace libuv)
endif()
