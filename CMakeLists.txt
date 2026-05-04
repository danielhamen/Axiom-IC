cmake_minimum_required(VERSION 3.20)
project(AxiomIC)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(aic ${SOURCES})

enable_testing()
find_package(Python3 COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(
        NAME axiomic_tests
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/run_tests.py --aic $<TARGET_FILE:aic>
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
