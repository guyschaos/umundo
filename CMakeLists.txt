cmake_minimum_required(VERSION 2.8.2)

project(umundo)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/contrib/cmake)

OPTION(BUILD_UMUNDO_CORE "Build umundo core" ON) 
OPTION(BUILD_UMUNDO_S11N "Build umundo serialization" ON) 

if (BUILD_UMUNDO_CORE)
	add_subdirectory(core)
	add_subdirectory(s11n)
endif()