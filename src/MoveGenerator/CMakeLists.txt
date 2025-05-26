#MoveGenerator

file(GLOB MG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB MG_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(MoveGeneratorLib STATIC ${MG_SOURCES} ${MG_HEADERS})

target_include_directories(MoveGeneratorLib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/MagicBitboards
)


target_include_directories(MoveGeneratorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(MoveGeneratorLib PRIVATE BoardRepresentationLib TTsLib)