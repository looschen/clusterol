cmake_minimum_required (VERSION 2.6)
project (clusterol)
set (clusterol_VERSION_MAJOR 0)
set (clusterol_VERSION_MINOR 1)

set (CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")
if (NOT CMAKE_BUILD_TYPE)
  # message(STATUS "** No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif()

include_directories ("${PROJECT_SOURCE_DIR}/include")

set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "Use static Boost libraries")
# set(Boost_USE_MULTITHREADED ON) 
# set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS program_options)
MESSAGE(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})


add_subdirectory (include)
add_subdirectory (clusterol-tool)

