

include_directories(.)
add_subdirectory(clientgame)
add_subdirectory(input)
add_subdirectory(video)

# Game client executable
set(target_NAME proto)
add_executable(${target_NAME} main.cpp ${client_SRC})
target_link_libraries(${target_NAME}
	${OGRE_LIBRARIES}
	${OIS_LIBRARIES}
	${BULLET_LIBRARIES}
	${shared_LIB}
)


# Copy media assets
add_custom_command(
	TARGET ${target_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_SOURCE_DIR}/media"
	"$<TARGET_FILE_DIR:${target_NAME}>/media"
)

# Copy dynamic libraries
macro(copy _file)
	add_custom_command(
		TARGET ${target_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${_file}"
		$<TARGET_FILE_DIR:${target_NAME}>
	)
endmacro()
if(MSVC)
	#copy(${CEGUIDIR}/bin/CEGUIBase.dll)
	#copy(${CEGUIDIR}/bin/CEGUIOgreRenderer.dll)
	copy(${OGRE_PLUGIN_DIR_REL}/OgreMain.dll)
	copy(${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll)
	copy(${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll)
	copy(${OGRE_PLUGIN_DIR_REL}/OIS.dll)

	# For some reason MSVC links both release and debug ogre/ois dlls
	copy(${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll)
	copy(${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll)
	copy(${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll)
	copy(${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll)
endif()
if(UNIX)
	list(GET OGRE_RenderSystem_GL_LIBRARIES 1 OGRE_RenderSystem_GL)
	copy(${OGRE_RenderSystem_GL})
endif()
