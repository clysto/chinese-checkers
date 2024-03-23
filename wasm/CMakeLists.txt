cmake_minimum_required(VERSION 3.15)

project(ChineseCheckers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(spdlog_DIR /opt/spdlog-1.x-wasm/lib/cmake/spdlog)

find_package(spdlog REQUIRED)

get_filename_component(ROOT_SOURCE_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)

add_executable(chinesecheckers
    wasm.cpp
    ${ROOT_SOURCE_DIR}/src/game.cpp
    ${ROOT_SOURCE_DIR}/src/game.hpp
    ${ROOT_SOURCE_DIR}/src/transtable.cpp
    ${ROOT_SOURCE_DIR}/src/transtable.hpp
)
target_compile_definitions(chinesecheckers PUBLIC -DHAVE_SPDLOG)
target_link_libraries(chinesecheckers embind spdlog::spdlog)
target_include_directories(chinesecheckers PRIVATE ${ROOT_SOURCE_DIR}/src)
set_target_properties(chinesecheckers PROPERTIES
    SUFFIX ".js"
    LINK_FLAGS "-s STACK_SIZE=256MB -s INITIAL_MEMORY=512MB -s WASM_BIGINT"
)

install(FILES $<TARGET_FILE_DIR:chinesecheckers>/chinesecheckers.js DESTINATION .)
install(FILES $<TARGET_FILE_DIR:chinesecheckers>/chinesecheckers.wasm DESTINATION .)
