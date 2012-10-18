

cmake_minimum_required(VERSION 2.8)

project(pre-proto)

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

# Find ogre's cmake modules ###################################################
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
endif (UNIX)



# Some build type stuff #######################################################
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")



# Find ogre and ois ###########################################################
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
if (NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif ()



# Find Boost ##################################################################
if (WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS TRUE)
else ()
	# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
	set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
endif ()

# Components that need linking
set(OGRE_BOOST_COMPONENTS thread date_time system)
set(GAME_BOOST_COMPONENTS filesystem chrono program_options)

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

# Find it
find_package(Boost REQUIRED ${OGRE_BOOST_COMPONENTS} ${GAME_BOOST_COMPONENTS})
if (NOT Boost_FOUND)
	# Try again with the other type of libs
	message("Boost wasn't found, trying again with the other link type")
	set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
	find_package(Boost REQUIRED COMPONENTS ${OGRE_BOOST_COMPONENTS} ${GAME_BOOST_COMPONENTS})
	if (NOT Boost_FOUND)
		message(SEND_ERROR "Failed to find Boost.")
	endif ()
endif ()

# Set up referencing of Boost
include_directories(${Boost_INCLUDE_DIR})
add_definitions(-DBOOST_ALL_NO_LIB)
set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})



# Define a macro to setup the sources #########################################
macro (add_sources)
	file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${ARGN})
		if(_relPath)
			list(APPEND preproto_SRC "${_relPath}/${_src}")
		else()
			list(APPEND preproto_SRC "${_src}")
		endif()
	endforeach()
	if(_relPath)
		# Propagate to parent
		set(preproto_SRC ${preproto_SRC} PARENT_SCOPE)
	endif()
endmacro()

# Define a macro so submodules can require their own libraries. Use it like
# target_link_libraries
macro (require_libraries)
	foreach(_lib ${ARGN})
		list(APPEND preproto_USERLIBS "${_lib}")
	endforeach()
	set(preproto_USERLIBS ${preproto_USERLIBS} PARENT_SCOPE)
endmacro()
macro (propagate_libs)
	set(preproto_USERLIBS ${preproto_USERLIBS} PARENT_SCOPE)
endmacro()

# Add sources and custom libraries
set(preproto_SRC "")
add_subdirectory(src)

# Source files
# set(HDRS
# 	./BaseApplication.h
# 	./TutorialApplication.h
# 	)
# set(SRCS
# 	./BaseApplication.cpp
# 	./TutorialApplication.cpp
# 	)

include_directories(
	${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_SAMPLES_INCLUDEPATH}
	)

#add_executable(${CMAKE_PROJECT_NAME} WIN32 ${HDRS} ${SRCS})
add_executable(${CMAKE_PROJECT_NAME} WIN32 ${preproto_SRC})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(${CMAKE_PROJECT_NAME} ${OGRE_LIBRARIES} ${OIS_LIBRARIES})

# Add propagated libraries
foreach (_lib ${preproto_USERLIBS})
	target_link_libraries(${CMAKE_PROJECT_NAME} ${_lib})
endforeach ()



# File Installations ##########################################################
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

if (MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist)
endif ()

# Post-build copy for win32
if (WIN32 AND NOT MINGW)
	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
		COMMAND if not exist .\\dist mkdir .\\dist )
	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND copy \"$(TargetPath)\" .\\dist )
endif ()

# Executables
install(TARGETS
	${CMAKE_PROJECT_NAME}
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




 
