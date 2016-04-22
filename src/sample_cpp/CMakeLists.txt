file(GLOB files *.cpp *.h)
add_executable(sample_cpp ${files})
target_link_libraries(sample_cpp ayxiatrace)
