cmake_minimum_required(VERSION 3.24.3)
project(NaezanCraft)

# Defulat mode is Debug
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

message(STATUS "Build type is set to ${CMAKE_BUILD_TYPE}")

# std=c++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# path
set(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/Binaries/Debug)

# editor
file(GLOB_RECURSE SRC_ALL Editor/Source/*.cpp Editor/Source/*.h NaezanCraft/Source/*.cpp NaezanCraft/Source/*.h)
add_executable(Editor ${SRC_ALL})
target_include_directories(Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/spdlog/include)

# set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# engine
file(GLOB_RECURSE SRC_ENGINE NaezanCraft/Source/*.cpp NaezanCraft/Source/*.h)
add_library(NaezanCraft SHARED ${SRC_ENGINE})
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/spdlog/include)

# set_target_properties(NaezanCraft PROPERTIES RUNTIME_LIBRARY_DIRECTORY ${OUTPUT_DIR})

# both
add_compile_definitions(EDITOR_ENGINE_DLL)

# intermediate dir is not not support