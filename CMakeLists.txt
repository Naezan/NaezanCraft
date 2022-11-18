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

# set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# engine
file(GLOB_RECURSE SRC_ENGINE NaezanCraft/Source/*.cpp NaezanCraft/Source/*.h)
add_executable(NaezanCraft ${SRC_ENGINE})
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/spdlog/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/glfw/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/GLAD/include)

target_precompile_headers(NaezanCraft PRIVATE NaezanCraft/Source/pch.h)

# glfw (do not build glfw)
add_subdirectory(NaezanCraft/ThirdParty/glfw EXCLUDE_FROM_ALL)

# GLAD (do not build GLAD)
add_subdirectory(NaezanCraft/ThirdParty/GLAD EXCLUDE_FROM_ALL)

# linking
target_link_libraries(NaezanCraft PRIVATE glfw glad)

# set_target_properties(NaezanCraft PROPERTIES RUNTIME_LIBRARY_DIRECTORY ${OUTPUT_DIR})

# both
add_compile_definitions(EDITOR_ENGINE_DLL)

# intermediate dir is not supporting