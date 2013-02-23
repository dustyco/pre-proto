

#add_subdirectory(entities)

# Add source files names to groups
add_sources(shared
	base.cpp
	render.cpp
	physics.cpp
)

# Add library names to groups
#require_libraries(groupname libraryname)

# Call last to pass group variables up to the parent
propagate_groups()
