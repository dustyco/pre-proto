

cmake_minimum_required(VERSION 2.8)

project(pre-proto CXX)

# Hide a a strange bug where ogre and boost headers explode with warnings
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275")
endif()

# Build in GNU profiling
# Other compiler id's: Clang, GNU, Intel, MSVC
if(PROFILE)
#	if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
#		message("Building in C GNU profiling")
#		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
#	else()
#		message("Requested C profiling but we don't have it for this compiler: " ${CMAKE_C_COMPILER_ID})
#	endif()
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
		set(PCH_PAIRS ${PCH_PAIRS} PARENT_SCOPE)
		set(_groups ${_groups} PARENT_SCOPE)
		foreach(_group ${_groups})
			set(${_group}_SRC ${${_group}_SRC} PARENT_SCOPE)
			set(${_group}_LIB ${${_group}_LIB} PARENT_SCOPE)
		endforeach()
	endif()
endmacro()

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


# Create PCH files and targets ######################################################################
if("${PCH}" STREQUAL "")
	set(PCH TRUE)
endif()
macro(make_pch_files)
	if(PCH)
		file(GLOB _pch_files "${CMAKE_SOURCE_DIR}/src/pch/*.h")
		message("_pch_files:")
		if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			foreach(_pch_file ${_pch_files})
				message("	${_pch_file}")
				GET_FILENAME_COMPONENT(_pch_name ${_pch_file} NAME_WE)
				message("	${_pch_name}")
	
				SET(_outdir "${CMAKE_BINARY_DIR}/pch")
				MAKE_DIRECTORY("${CMAKE_BINARY_DIR}/pch")
				SET(_output "${_outdir}/${_pch_name}.h.gch")
	
				STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
				SET(_compiler_FLAGS ${${_flags_var_name}})

				GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
				FOREACH(item ${_directory_flags})
					LIST(APPEND _compiler_FLAGS "-I${item}")
				ENDFOREACH(item)
			 
				GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
				LIST(APPEND _compiler_FLAGS ${_directory_flags})
			 
				SEPARATE_ARGUMENTS(_compiler_FLAGS)
				MESSAGE("${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_pch_file}")
				ADD_CUSTOM_COMMAND(
					OUTPUT ${_output}
					COMMAND ${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_pch_file}
					DEPENDS ${_source}
				)
#				ADD_CUSTOM_TARGET(${_pch_name} DEPENDS ${_output})
			endforeach()
		endif()
		
		list(LENGTH PCH_PAIRS _length)
		while(NOT ${_length} STREQUAL "0")
			list(GET PCH_PAIRS 0 _pch_name)
			list(GET PCH_PAIRS 1 _source)
			list(REMOVE_AT PCH_PAIRS 0 1)
			list(LENGTH PCH_PAIRS _length)
			set_property(
				SOURCE "${_source}"
				APPEND
				PROPERTY COMPILE_FLAGS "-D PCH -include-pch ${CMAKE_BINARY_DIR}/pch/${_pch_name}.h.gch"
			)
			set_property(
				SOURCE "${_source}"
				APPEND
				PROPERTY OBJECT_DEPENDS "${CMAKE_BINARY_DIR}/pch/${_pch_name}.h.gch"
			)
		endwhile()
	endif()
endmacro()

# Include a precompiled header (src/pch/*.h) to source files
# Usage: include_pch([pch name] [source filename]...)
#        After, call propagate_groups() once in each directory
macro(include_pch)
	set(_arguments ${ARGN})
	list(GET _arguments 0 _pch)
	list(REMOVE_AT _arguments 0)
	foreach(_source ${_arguments})
		list(APPEND PCH_PAIRS "${_pch}")
		list(APPEND PCH_PAIRS "${CMAKE_CURRENT_SOURCE_DIR}/${_source}")
	endforeach()
endmacro()




# Work through the source tree
add_subdirectory(src)
make_pch_files()


# Shared static library so we don't have to redundantly compile shared code
add_library(shared STATIC ${shared_SRC})

# Game client executable
set(CLIENT_EXECUTABLE proto)
add_executable(${CLIENT_EXECUTABLE} ${client_SRC})
target_link_libraries(${CLIENT_EXECUTABLE} ${client_LIB} shared ${shared_LIB})
set_target_properties(${CLIENT_EXECUTABLE} PROPERTIES DEBUG_POSTFIX _d)

# Game server executable
set(SERVER_EXECUTABLE proto-server)
add_executable(${SERVER_EXECUTABLE} ${server_SRC})
target_link_libraries(${SERVER_EXECUTABLE} ${server_LIB} shared ${shared_LIB})


# File Installations ##########################################################

#file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
#file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/media)
add_custom_command(
	TARGET shared POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/media"
		"$<TARGET_FILE_DIR:shared>/media"
)

if(UNIX)
	list(GET OGRE_RenderSystem_GL_LIBRARIES 1 OGRE_RenderSystem_GL_LINK)
	GET_FILENAME_COMPONENT(OGRE_RenderSystem_GL_FILE ${OGRE_RenderSystem_GL_LINK} REALPATH)
	add_custom_command(
		TARGET ${CLIENT_EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${OGRE_RenderSystem_GL_LINK}
			"$<TARGET_FILE_DIR:${CLIENT_EXECUTABLE}>"
	)
endif()

