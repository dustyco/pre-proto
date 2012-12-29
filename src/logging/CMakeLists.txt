

# Add source files names to groups
add_sources(shared
	ConsoleLogger.cpp
	LogManager.cpp
)
add_sources(client OgreLogAdapter.cpp)

# Add library names to groups
#require_libraries(groupname libraryname)

# Call last to pass group variables up to the parent
propagate_groups()
