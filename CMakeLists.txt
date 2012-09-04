

cmake_minimum_required(VERSION 2.6)

project(pre-proto)

# Find ogre's cmake modules
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


# Some build type stuff
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")


# Find ogre and ois
find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)
if (NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif ()

# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
	    set(Boost_USE_STATIC_LIBS TRUE)
	else ()
	    # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
	    set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	
	if (MINGW)
	    # this is probably a bug in CMake: the boost find module tries to look for
	    # boost libraries with name libboost_*, but CMake already prefixes library
	    # search names with "lib". This is the workaround.
	    set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time system)
	set(GAME_BOOST_COMPONENTS filesystem chrono program_options)
	find_package(Boost 1.50.0 COMPONENTS ${OGRE_BOOST_COMPONENTS} ${GAME_BOOST_COMPONENTS} QUIET)
	
	if (NOT Boost_FOUND)
	    # Try again with the other type of libs
	    set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
	    find_package(Boost 1.5.0 COMPONENTS ${OGRE_BOOST_COMPONENTS} ${GAME_BOOST_COMPONENTS} QUIET)
	endif ()
	
	if (WIN32)
	    # Get boost from the ogre sdk
	    set(BOOST_ROOT $ENV{OGRE_HOME}/boost)
	    message("BOOST_ROOT: " ${BOOST_ROOT})
	endif ()
	find_package(Boost REQUIRED)
 
	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif ()

# Define a macro to setup the sources
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
foreach(_lib ${preproto_USERLIBS})
	target_link_libraries(${CMAKE_PROJECT_NAME} ${_lib})
endforeach()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)

# post-build copy for win32
if (WIN32 AND NOT MINGW)
	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} PRE_BUILD
	    COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
	add_custom_command( TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
	    COMMAND copy \"$(TargetPath)\" .\\dist\\bin )
endif (WIN32 AND NOT MINGW)

if (MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif (MINGW OR UNIX)

if (WIN32)
	install(TARGETS
		${CMAKE_PROJECT_NAME}
		RUNTIME DESTINATION bin
		CONFIGURATIONS All
		)
	install(DIRECTORY
		${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION .
		CONFIGURATIONS Release RelWithDebInfo Debug
		)
	install(FILES
		${CMAKE_SOURCE_DIR}/dist/bin/cfg-windows/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
		)
	install(FILES
		${CMAKE_SOURCE_DIR}/dist/bin/cfg-windows/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources_d.cfg
		DESTINATION bin
		CONFIGURATIONS Debug
		)
	install(FILES
		${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
		)
	install(FILES
		${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		DESTINATION bin
		CONFIGURATIONS Debug
		)
# Doesn't seem to need these
# Keeping them just in case
#	install(FILES
#		${Boost_DATE_TIME_LIBRARY_RELEASE}
#		${Boost_THREAD_LIBRARY_RELEASE}
#		DESTINATION bin
#		CONFIGURATIONS Release RelWithDebInfo
#		)
#	install(FILES
#		${Boost_DATE_TIME_LIBRARY_DEBUG}
#		${Boost_THREAD_LIBRARY_DEBUG}
#		DESTINATION bin
#		CONFIGURATIONS Debug
#		)
endif (WIN32)

if (UNIX)
	install(TARGETS
		${CMAKE_PROJECT_NAME}
		RUNTIME DESTINATION bin
		CONFIGURATIONS All
		)
	install(DIRECTORY
		${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION .
		CONFIGURATIONS Release RelWithDebInfo Debug
		)
	install(FILES
		${CMAKE_SOURCE_DIR}/dist/bin/cfg-linux/plugins.cfg
		${CMAKE_SOURCE_DIR}/dist/bin/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo Debug
		)
# Linux builds aren't being distributed so let ogre find
# the plugins with the config file
#	install(FILES
#		/usr/local/lib/OGRE/RenderSystem_GL.so
#		DESTINATION bin
#		CONFIGURATIONS Release RelWithDebInfo
#		)
endif (UNIX)
 
