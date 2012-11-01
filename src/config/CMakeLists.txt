

# Add source files names to groups
add_sources(client etf.cpp ConfigManager.cpp Tree.cpp)

# Add library names to groups
require_libraries(client boost_filesystem boost_program_options)

# Call last to pass group variables up to the parent
propagate_groups()
