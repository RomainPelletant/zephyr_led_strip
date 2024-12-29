add_subdirectory(pattern)
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/led_player.c
)