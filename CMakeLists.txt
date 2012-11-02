

cmake_minimum_required(VERSION 2.8)

project(pre-proto CXX)

# Hide a a strange bug where ogre and boost headers explode with warnings
if (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275")
endif ()

# Build in GNU profiling
# Other compiler id's: Clang, GNU, Intel, MSVC
if (PROFILE)
	if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
		message("Building in C GNU profiling")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
	endif ()
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message("Building in CXX GNU profiling")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	endif ()
endif ()


# Some build type stuff #######################################################
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")



# Subdirectories can create source file groups
# Usage: add_sources([group name] [source file name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_SRC will contain the source file paths
macro(add_sources)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${_arguments})
		if(_relPath)
			list(APPEND ${_group}_SRC "${_relPath}/${_src}")
		else()
			list(APPEND ${_group}_SRC "${_src}")
		endif()
	endforeach()
endmacro()

# Subdirectories can create external library groups
# Usage: require_libraries([group name] [library name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_LIB will contain the library names
macro (require_libraries)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _group)
	list(REMOVE_AT _arguments 0)
	list(APPEND _groups ${_group})
	foreach(_lib ${_arguments})
		list(APPEND ${_group}_LIB "${_lib}")
	endforeach()
endmacro()

# Call this in each subdirectory AFTER all group related calls
macro(propagate_groups)
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	if(_relPath)
		set(_groups ${_groups} PARENT_SCOPE)
		foreach(_group ${_groups})
			set(${_group}_SRC ${${_group}_SRC} PARENT_SCOPE)
			set(${_group}_LIB ${${_group}_LIB} PARENT_SCOPE)
		endforeach()
	endif()
endmacro()



# Find Ogre ###########################################################
# Locate its cmake modules
if (WIN32)
	message("OGRE_HOME: " $ENV{OGRE_HOME})
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif (WIN32)
if(UNIX)
	if (EXISTS "/usr/local/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif (EXISTS "/usr/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	else ()
		message(SEND_ERROR "Failed to find module path.")
	endif (EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)
find_package(OGRE REQUIRED)
if(NOT OGRE_FOUND)
	message(SEND_ERROR "Failed to find Ogre.")
endif()
include_directories(${OGRE_INCLUDE_DIRS})

# Find OIS ###########################################################
find_package(OIS REQUIRED)
if (NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif ()
include_directories(${OIS_INCLUDE_DIRS})

# Find CEGUI ##################################################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH}")
find_package(CEGUI REQUIRED COMPONENTS OGRE)
if (NOT CEGUI_FOUND)
	message(SEND_ERROR "Failed to find CEGUI.")
endif ()
include_directories(${CEGUI_INCLUDE_DIR})



# Find Boost ##################################################################
if (WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS TRUE)
else ()
	# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
	set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
endif ()

# Decide where to look
set(BOOST_ROOT_ENV $ENV{BOOST_ROOT})
if (BOOST_ROOT_ENV)
	# Get boost from a specifically defined folder
	set(BOOST_ROOT $ENV{BOOST_ROOT})
	message("BOOST_ROOT: " ${BOOST_ROOT})
elseif (WIN32)
	# Get boost from the ogre sdk
	set(BOOST_ROOT $ENV{OGRE_HOME}/boost)
	message("BOOST_ROOT: " ${BOOST_ROOT})
endif ()

# Specify which linkable libraries we should find
set(BOOST_COMPONENTS
	thread
	date_time
	system
	filesystem
	program_options
	chrono
)

# Find it
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
if (NOT Boost_FOUND)
	# Try again with the other type of libs
	message("Boost wasn't found, trying again with the other link type")
	set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
	find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
	if (NOT Boost_FOUND)
		message(SEND_ERROR "Failed to find Boost.")
	endif ()
endif ()

# Set up referencing of Boost
include_directories(${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_NO_LIB)

# Add the boost libraries ogre uses
set(OGRE_LIBRARIES
	${OGRE_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)



# Work through the source tree
add_subdirectory(src)



# Shared static library so we don't have to redundantly compile shared code
#add_library(shared STATIC ${shared_SRC})

# Game client executable
set(CLIENT_EXECUTABLE ${CMAKE_PROJECT_NAME})
add_executable(${CLIENT_EXECUTABLE} ${client_SRC})
target_link_libraries(${CLIENT_EXECUTABLE} ${client_LIB}) # shared ${shared_LIB})
set_target_properties(${CLIENT_EXECUTABLE} PROPERTIES DEBUG_POSTFIX _d)

# Game server executable
#set(SERVER_EXECUTABLE ${CMAKE_PROJECT_NAME}_server)
#add_executable(${SERVER_EXECUTABLE} ${server_SRC})
#target_link_libraries(${SERVER_EXECUTABLE} ${server_LIB} shared ${shared_LIB})



# File Installations ##########################################################
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

if (MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist)
endif ()

# Post-build copy for win32
#if (WIN32 AND NOT MINGW)
#	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
#		COMMAND if not exist .\\dist mkdir .\\dist )
#	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
#		COMMAND copy \"$(TargetPath)\" .\\dist )
#endif ()

# Executables
install(TARGETS
	${CLIENT_EXECUTABLE}
	RUNTIME DESTINATION .
	)
install(DIRECTORY
	${CMAKE_SOURCE_DIR}/media
	DESTINATION .
	)

# Plugins
if (WIN32)
	install(FILES
		${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		DESTINATION .
		CONFIGURATIONS Release RelWithDebInfo
		)
	install(FILES
		${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		DESTINATION .
		CONFIGURATIONS Debug
		)
endif (WIN32)
if (UNIX)
	list(GET OGRE_RenderSystem_GL_LIBRARIES 1 OGRE_RenderSystem_GL_LINK)
	GET_FILENAME_COMPONENT(OGRE_RenderSystem_GL_FILE ${OGRE_RenderSystem_GL_LINK} REALPATH)
	install(FILES
		${OGRE_RenderSystem_GL_FILE}
		${OGRE_RenderSystem_GL_LINK}
		DESTINATION .
		)
endif (UNIX)




 
