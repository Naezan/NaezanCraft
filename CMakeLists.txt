cmake_minimum_required(VERSION 3.24.3)
project(NaezanCraft)

# Defulat mode is Debug
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

message(STATUS "Build type is set to ${CMAKE_BUILD_TYPE}")

# std=c++20
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# path
set(OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/Binaries/Debug)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# set_target_properties(Editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(FASTNOISE_OPTION ON/OFF CACHE BOOL "Description of option" FORCE)

# engine
file(GLOB_RECURSE SRC_ENGINE NaezanCraft/Source/*.cpp NaezanCraft/Source/*.h)
add_executable(NaezanCraft ${SRC_ENGINE})
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/spdlog/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/glfw/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/GLAD/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/imgui)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/imgui/backends)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/glm)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/FastNoise2/include)

# pure lib path
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/Optick/include)
target_include_directories(NaezanCraft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/irrKlang/include)

# target_precompile_headers(NaezanCraft PRIVATE NaezanCraft/Source/pch.h)

# glfw (do not build glfw)
add_subdirectory(NaezanCraft/ThirdParty/glfw EXCLUDE_FROM_ALL)

# GLAD (do not build GLAD)
add_subdirectory(NaezanCraft/ThirdParty/GLAD EXCLUDE_FROM_ALL)

# imgui (do not build imgui)
add_subdirectory(NaezanCraft/ThirdParty/imgui EXCLUDE_FROM_ALL)

# glm (do not build glm)
add_subdirectory(NaezanCraft/ThirdParty/glm EXCLUDE_FROM_ALL)

# FastNoise2
add_subdirectory(NaezanCraft/ThirdParty/FastNoise2)

# linking
target_link_libraries(NaezanCraft PUBLIC glfw glad imgui glm ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/Optick/lib/x64/debug/OptickCore.lib ${CMAKE_CURRENT_SOURCE_DIR}/NaezanCraft/ThirdParty/irrKlang/lib/irrKlang.lib)
target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(NaezanCraft PUBLIC FastNoise)

add_custom_command(
        TARGET NaezanCraft PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/Libs/OptickCore.dll ${CMAKE_BINARY_DIR}/Debug
)
add_custom_command(
        TARGET NaezanCraft PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/Libs/ikpFlac.dll ${CMAKE_BINARY_DIR}/Debug
)
add_custom_command(
        TARGET NaezanCraft PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/Libs/ikpMP3.dll ${CMAKE_BINARY_DIR}/Debug
)
add_custom_command(
        TARGET NaezanCraft PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/Libs/irrKlang.dll ${CMAKE_BINARY_DIR}/Debug
)

# set_target_properties(NaezanCraft PROPERTIES RUNTIME_LIBRARY_DIRECTORY ${OUTPUT_DIR})

# both
add_compile_definitions(EDITOR_ENGINE_DLL)

# intermediate dir is not supporting