cmake_minimum_required(VERSION 3.16)

project(raw_pdb)

set(CMAKE_CXX_STANDARD 11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
	add_compile_options(-fdeclspec
						-fms-extensions)
endif()

add_subdirectory(src)