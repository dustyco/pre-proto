

#add_subdirectory(entity)

# Add source files names to groups
add_sources(client
	ClientGame.cpp
)

# Add library names to groups
#require_libraries(groupname libraryname)

# Call last to pass group variables up to the parent
propagate_groups()
