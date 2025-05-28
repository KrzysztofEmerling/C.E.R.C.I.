file(GLOB TEST_MG_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/MoveGenerator/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ZobristHash/zHashCalculationTests.cpp")

add_library(TestLib STATIC ${TEST_MG_SOURCES})

# Nagłówki testów
target_include_directories(TestLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/MoveGenerator
    ${CMAKE_CURRENT_SOURCE_DIR}/ZobristHash

    ${CMAKE_SOURCE_DIR}/src/BoardRepresentation
    ${CMAKE_SOURCE_DIR}/src/MoveGenerator
    ${CMAKE_SOURCE_DIR}/src/TranspositionTabels
)

# Linkowanie z bibliotekami silnika
target_link_libraries(TestLib
    BoardRepresentationLib
    MoveGeneratorLib
    TTsLib 
)
