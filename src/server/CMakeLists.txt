

include_directories(.)
#add_subdirectory(dir)

# Game server executable
set(target_NAME proto_server)
add_executable(${target_NAME} main.cpp ${server_SRC})
target_link_libraries(${target_NAME}
	${shared_LIB}
)
