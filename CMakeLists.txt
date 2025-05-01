cmake_minimum_required(VERSION 3.16)
project(CERCI-ChessEngine)

set(CMAKE_CXX_STANDARD 23)

option(BUILD_TESTS "Build the tests" OFF)

# Pliki źródłowe
file(GLOB_RECURSE DEBUG_FILES src/Debuging/*.cpp)
file(GLOB_RECURSE MG_FILES src/MoveGenerator/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/MoveGenerator/*.cpp)

# ===> 1. Tworzenie statycznej biblioteki z MoveGenerator
add_library(movegenerator STATIC ${MG_FILES})
target_include_directories(movegenerator PUBLIC src)

# Główna aplikacja lub testy
if(BUILD_TESTS)
    message(STATUS "Building tests")
    add_executable(${PROJECT_NAME} ${TEST_FILES} ${DEBUG_FILES} tests/tests.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE src tests)
    target_link_libraries(${PROJECT_NAME} movegenerator)
else()
    message(STATUS "Building application")
    add_executable(${PROJECT_NAME} ${DEBUG_FILES} src/main.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE src)
    target_link_libraries(${PROJECT_NAME} movegenerator)
endif()

# Domyślny typ budowy
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Flagi kompilatora
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
