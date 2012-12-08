

# Add source files names to groups
add_sources(client
	timer.cpp
	fsutil.cpp
	hex.cpp
	Clock.cpp
)

# Add library names to groups
require_libraries(client
	${Boost_DATE_TIME_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_CHRONO_LIBRARY}
)
if(UNIX)
	require_libraries(client rt)
endif()

# Call last to pass group variables up to the parent
propagate_groups()
