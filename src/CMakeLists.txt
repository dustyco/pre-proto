

add_subdirectory(config)
add_subdirectory(util)
#add_subdirectory(profiler)
add_subdirectory(logging)
add_subdirectory(input)
#add_subdirectory(game)
add_subdirectory(video)
add_subdirectory(gui)

# Add source file names to groups
add_sources(client main.cpp Application.cpp)

# Add library names to groups
require_libraries(client
	${OGRE_LIBRARIES}
	${OIS_LIBRARIES}
	${CEGUI_LIBRARY}
	)
if(UNIX)
	require_libraries(client pthread)
endif()

# Call last to pass group variables up to the parent
propagate_groups()
