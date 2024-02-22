cmake_minimum_required(VERSION 3.15)

project(ChineseCheckers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(chinesecheckers src/wasm.cpp src/game.cpp src/game.hpp)
target_link_libraries(chinesecheckers embind)
target_include_directories(chinesecheckers PRIVATE ${CMAKE_SOURCE_DIR}/src)
set_target_properties(chinesecheckers PROPERTIES
    SUFFIX ".js"
    LINK_FLAGS "-s STACK_SIZE=16MB -s INITIAL_MEMORY=32MB"
)

add_custom_command(TARGET chinesecheckers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:chinesecheckers>/chinesecheckers.js ${CMAKE_SOURCE_DIR}/web/
)
add_custom_command(TARGET chinesecheckers POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:chinesecheckers>/chinesecheckers.wasm ${CMAKE_SOURCE_DIR}/web/
)

add_executable(chinesecheckers_server src/server.cpp src/httplib.h src/game.cpp src/game.hpp)
target_include_directories(chinesecheckers_server PRIVATE ${CMAKE_SOURCE_DIR}/src)
