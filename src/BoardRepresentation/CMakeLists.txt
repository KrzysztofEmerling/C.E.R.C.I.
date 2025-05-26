#BoardReprezentation

file(GLOB BR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB BR_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(BoardRepresentationLib STATIC ${BR_SOURCES} ${BR_HEADERS})

# Dodaj katalogi nagłówków
target_include_directories(BoardRepresentationLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
