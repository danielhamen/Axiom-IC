cmake_minimum_required(VERSION 3.20)
project(AxiomIC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(aic ${SOURCES})
