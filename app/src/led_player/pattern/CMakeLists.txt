add_subdirectory(types)
target_sources(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/generic.c
)