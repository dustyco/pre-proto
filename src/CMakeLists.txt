add_subdirectory(config)
add_subdirectory(util)
add_subdirectory(profiler)
add_subdirectory(logging)
add_subdirectory(input)
add_subdirectory(game)
add_subdirectory(video)

# Add our own sources
add_sources(main.cpp Application.cpp)

# Propagate sub-libraries
require_libraries(pthread)
