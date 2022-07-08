cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(MURStrategy VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	add_compile_options(/utf-8 /W2 /MP /Gm-)
endif()

add_subdirectory(./src)