cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mkatlas)

option(DISABLE_FREETYPE "Disable Freetype")

find_package(PNG REQUIRED)

if (NOT ${DISABLE_FREETYPE})
	find_package(Freetype)
	
	if (NOT ${FREETYPE_FOUND})
		message(WARNING "Application will be compiled with DISABLE_FREETYPE")
		set(DISABLE_FREETYPE ON)
	endif()
endif()

include_directories("src" ${PNG_INCLUDE_DIRS})

if (NOT ${DISABLE_FREETYPE})
	include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

add_definitions(${PNG_DEFINITIONS})

if (${DISABLE_FREETYPE})
	add_definitions(-DDISABLE_FREETYPE)
endif()

project(mkatlas)
file(GLOB_RECURSE MKATLASSRC "src/*.cpp" "src/*.hpp")
add_executable(mkatlas ${MKATLASSRC})
target_link_libraries(mkatlas ${PNG_LIBRARIES})

if (NOT ${DISABLE_FREETYPE})
	target_link_libraries(mkatlas ${FREETYPE_LIBRARIES})
endif()
