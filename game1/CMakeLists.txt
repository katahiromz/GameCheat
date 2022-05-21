add_executable(game1 WIN32 game1.cpp)
target_compile_options(game1 PRIVATE -O0)
add_custom_command(TARGET game1 POST_BUILD
    COMMAND $<TARGET_FILE:checksum> --remove $<TARGET_FILE:game1>)
