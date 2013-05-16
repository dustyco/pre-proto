

add_subdirectory(config)
add_subdirectory(logging)
#add_subdirectory(profiler)
add_subdirectory(sim)
add_subdirectory(util)

# Shared static library so we don't have to redundantly compile shared code
add_library(shared ${shared_SRC})
require_libraries(shared shared)
if(UNIX)
	require_libraries(shared pthread)
endif()

propagate_groups()
