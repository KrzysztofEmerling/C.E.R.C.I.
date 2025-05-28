#TranspositionsTables

file(GLOB TT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB TT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(TTsLib STATIC ${TT_SOURCES} ${TT_HEADERS})

# Dodaj katalogi nagłówków
target_include_directories(TTsLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/BoardRepresentation
)
