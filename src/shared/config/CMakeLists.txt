

# Add source files names to groups
add_sources(shared
	ConfigManager.cpp
)

# Add library names to groups
require_libraries(shared
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

# Call last to pass group variables up to the parent
propagate_groups()
