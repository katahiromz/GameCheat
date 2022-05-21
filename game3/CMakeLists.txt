add_executable(game3 WIN32 game3.cpp game3_res.rc)
target_link_libraries(game3 PRIVATE comctl32)
add_custom_command(TARGET game3 POST_BUILD
    COMMAND $<TARGET_FILE:checksum> --add $<TARGET_FILE:game3>)
