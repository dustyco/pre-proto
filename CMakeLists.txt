

cmake_minimum_required(VERSION 2.8)
project(pre-proto CXX)


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
#		if(_relPath)
#			list(APPEND ${_group}_SRC "${_relPath}/${_src}")
#		else()
#			list(APPEND ${_group}_SRC "${_src}")
#		endif()
		list(APPEND ${_group}_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
	endforeach()
endmacro()

# Subdirectories can create external library groups
# Usage: require_libraries([group name] [library name]...)
#        After, call propagate_groups() once in each directory
#        The variable [group name]_LIB will contain the library names
macro(require_libraries)
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


if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	# Hide a a strange bug where ogre and boost headers explode with warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# Suppress switch warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
endif()


# Build in GNU profiling
# Other compiler id's: Clang, GNU, Intel, MSVC
if(PROFILE)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		message("Building in CXX GNU profiling")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	else()
		message("Requested CXX profiling but we don't have it for this compiler: " ${CMAKE_CXX_COMPILER_ID})
	endif()
endif()


# Build type
# Options: Debug, Release, RelWithDebInfo, MinSizeRel
# Only Debug, Release on MSVC
if(CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty
	if(UNIX)
		set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
	else()
		set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release." FORCE)
	endif()
	message("Default build type: " ${CMAKE_BUILD_TYPE})
else()
	message("Chosen build type: " ${CMAKE_BUILD_TYPE})
endif()
set(CMAKE_DEBUG_POSTFIX "_d")


# Find Ogre ###################################################################
# Locate its cmake modules
if(WIN32)
	message("OGRE_HOME: " $ENV{OGRE_HOME})
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)
if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "/usr/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	else()
		message(SEND_ERROR "Failed to find module path.")
	endif(EXISTS "/usr/local/lib/OGRE/cmake")
endif(UNIX)
find_package(OGRE REQUIRED)
include_directories(${OGRE_INCLUDE_DIRS})

# Find OIS ####################################################################
find_package(OIS REQUIRED)
include_directories(${OIS_INCLUDE_DIRS})

# Find MyGUI ##################################################################
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH}")
#find_package(MyGUI REQUIRED COMPONENTS OGRE)
#if(NOT MYGUI_FOUND)
#	message(SEND_ERROR "Failed to find MyGUI.")
#endif()
#include_directories(${MYGUI_INCLUDE_DIRS})
#message(MYGUI_INCLUDE_DIRS = ${MYGUI_INCLUDE_DIRS})
#message(MYGUI_LIBRARIES = ${MYGUI_LIBRARIES})
#message(MYGUI_PLATFORM_LIBRARIES = ${MYGUI_PLATFORM_LIBRARIES})

# Find CEGUI ##################################################################
#set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR};${CMAKE_MODULE_PATH}")
#find_package(CEGUI REQUIRED COMPONENTS OGRE)
#if(NOT CEGUI_FOUND)
#	message(SEND_ERROR "Failed to find CEGUI.")
#endif()
#include_directories(${CEGUI_INCLUDE_DIR})

# Find Bullet #################################################################
# Doesn't work on windows yet
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})

# Find Boost ##################################################################
if(Boost_USE_STATIC_LIBS STREQUAL "")
	set(Boost_USE_STATIC_LIBS TRUE)
endif()
set(BOOST_ROOT_ENV $ENV{BOOST_ROOT})
if(BOOST_ROOT_ENV)
	# Get boost from a specifically defined folder
	set(BOOST_ROOT $ENV{BOOST_ROOT})
	message("BOOST_ROOT: " ${BOOST_ROOT})
elseif(WIN32)
	# Get boost from the ogre sdk
	set(BOOST_ROOT $ENV{OGRE_HOME}/boost)
	message("BOOST_ROOT: " ${BOOST_ROOT})
endif()
set(BOOST_COMPONENTS
	thread
	date_time
	system
	filesystem
	program_options
	chrono
)
message("Boost_USE_STATIC_LIBS: ${Boost_USE_STATIC_LIBS}")
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
include_directories(${Boost_INCLUDE_DIR})

# Add the boost libraries ogre uses
set(OGRE_LIBRARIES
	${OGRE_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
)


# Work through the source tree
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)




