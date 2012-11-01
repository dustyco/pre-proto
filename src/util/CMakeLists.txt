

# Add source files names to groups
add_sources(client timer.cpp fsutil.cpp hex.cpp)

# Add library names to groups
require_libraries(client rt boost_chrono)

# Call last to pass group variables up to the parent
propagate_groups()
