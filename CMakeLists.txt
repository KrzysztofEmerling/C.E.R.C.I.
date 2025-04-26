cmake_minimum_required(VERSION 3.16)
project(CERCI-ChessEngine)

# Ustaw standard C++
set(CMAKE_CXX_STANDARD 23)

# Opcja wyboru trybu Test
option(BUILD_TESTS "Build the tests" OFF)

# Znajdź wszystkie pliki źródłowe
file(GLOB_RECURSE MG_FILES src/MoveGenerator/*.cpp)
file(GLOB_RECURSE TEST_FILES tests/MoveGenerator/*.cpp)

# Jeśli budujemy testy
if(BUILD_TESTS)
    message(STATUS "Building tests")
    add_executable(${PROJECT_NAME} ${MG_FILES} ${TEST_FILES} tests/tests.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE src tests)
else()
    message(STATUS "Building application")
    add_executable(${PROJECT_NAME} ${MG_FILES} src/main.cpp)
    target_include_directories(${PROJECT_NAME} PRIVATE src)
endif()

# Domyślny typ budowy
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Ustawienia kompilatora
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")