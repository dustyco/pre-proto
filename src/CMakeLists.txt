

add_subdirectory(clientgame)
add_subdirectory(config)
add_subdirectory(input)
add_subdirectory(logging)
#add_subdirectory(profiler)
add_subdirectory(servergame)
add_subdirectory(sim)
add_subdirectory(util)
add_subdirectory(video)

# Add source file names to groups
add_sources(client Client.cpp)
add_sources(server Server.cpp)

# Add library names to groups
require_libraries(client
	${OGRE_LIBRARIES}
	${OIS_LIBRARIES}
)
if (UNIX)
	require_libraries(shared pthread)
endif ()

# Call last to pass group variables up to the parent
propagate_groups()
