cmake_minimum_required(VERSION 3.24.3)
project(FastNoise)
set(CMAKE_CXX_STANDARD 20)

add_library(FastNoise include/FastSIMD/FastSIMD.h include/FastSIMD/FastSIMD_Config.h include/FastSIMD/FastSIMD_Export.h include/FastSIMD/FunctionList.h include/FastSIMD/InlInclude.h include/FastSIMD/SIMDTypeList.h include/FastNoise/FastNoise.h include/FastNoise/FastNoise_C.h include/FastNoise/FastNoise_Config.h include/FastNoise/FastNoise_Export.h include/FastNoise/Metadata.h include/FastNoise/SmartNode.h include/FastNoise/Generators/BasicGenerators.h include/FastNoise/Generators/Blends.h include/FastNoise/Generators/Cellular.h include/FastNoise/Generators/DomainWarp.h include/FastNoise/Generators/DomainWarpFractal.h include/FastNoise/Generators/Fractal.h include/FastNoise/Generators/Generator.h include/FastNoise/Generators/Modifiers.h include/FastNoise/Generators/Perlin.h include/FastNoise/Generators/Simplex.h include/FastNoise/Generators/Value.h)
target_include_directories(FastNoise PUBLIC include/)