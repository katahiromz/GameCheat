add_executable(game2 WIN32 game2.cpp)
target_compile_options(game2 PRIVATE -O0)
target_link_libraries(game2 PRIVATE imagehlp)
add_custom_command(TARGET game2 POST_BUILD
    COMMAND $<TARGET_FILE:checksum> --add $<TARGET_FILE:game2>)
