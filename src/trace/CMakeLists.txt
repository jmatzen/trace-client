file(GLOB files *.cpp *.h ../../include/trace/*.h)

add_library(ayxiatrace SHARED ${files})

if (WIN32)
  target_link_libraries(ayxiatrace libuv ws2_32 mincore psapi)
  add_definitions(/D_WIN32_WINNT=0x06010000)
endif()
