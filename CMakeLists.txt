cmake_minimum_required(VERSION 3.16)
project(CERCI-ChessEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build the tests" OFF)

add_subdirectory(src/BoardRepresentation)
add_subdirectory(src/MoveGenerator)
add_subdirectory(src/TranspositionTabels)
add_subdirectory(src/Eval)

# Budowa testów
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)

    message(STATUS "Building tests")
    add_executable(${PROJECT_NAME} tests/main.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE TestLib)

else()
    message(STATUS "Building application")
    add_executable(${PROJECT_NAME} src/main.cpp src/UCI.cpp)

endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    BoardRepresentationLib
    MoveGeneratorLib 
    TTsLib
    EvalLib
)

target_include_directories(${PROJECT_NAME} PRIVATE src/BoardRepresentation)

# Domyślny typ budowy
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Flagi kompilatora
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")

# Włączenie LTO (IPO) jeśli jest obsługiwane przez kompilator
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

if(ipo_supported)
    message(STATUS "IPO / LTO supported, enabling interprocedural optimization for Release build")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(STATUS "IPO / LTO not supported: ${ipo_error}")
endif()
