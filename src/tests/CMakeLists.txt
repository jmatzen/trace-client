file(GLOB files *.cpp *.h)
add_executable(tests ${files})
target_link_libraries(tests ayxiatrace UnitTest++)

