file(GLOB files *.c *.h)
add_executable(sample_c ${files})
target_link_libraries(sample_c ayxiatrace)
