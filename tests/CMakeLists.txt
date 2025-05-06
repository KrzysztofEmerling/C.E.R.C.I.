file(GLOB TEST_MG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/MoveGenerator/*.cpp")

add_library(TestLib STATIC ${TEST_MG_SOURCES})

# Dodaj katalogi nagłówków
target_include_directories(TestLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/MoveGenerator
    #Z MoveGeneratora silnika
    ${CMAKE_SOURCE_DIR}/src/BoardRepresentation
    ${CMAKE_SOURCE_DIR}/src/MoveGenerator
)