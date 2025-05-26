#Eval

file(GLOB Ev_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB Ev_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(EvalLib STATIC ${Ev_SOURCES} ${Ev_HEADERS})

# Dodaj katalogi nagłówków
target_link_libraries(EvalLib PUBLIC MoveGeneratorLib)
target_include_directories(EvalLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/BoardRepresentation
)

