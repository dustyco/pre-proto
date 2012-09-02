add_subdirectory(config)
add_subdirectory(util)
add_subdirectory(profiler)
add_subdirectory(logging)

# Add our own sources
add_sources(BaseApplication.cpp TutorialApplication.cpp)

# Propagate sub-libraries
require_libraries(pthread)
