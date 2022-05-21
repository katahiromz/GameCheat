add_executable(game2 game2.cpp)
target_compile_options(game2 PRIVATE -O0)
add_custom_command(TARGET game2 POST_BUILD
    COMMAND $<TARGET_FILE:checksum> --add $<TARGET_FILE:game2>)
