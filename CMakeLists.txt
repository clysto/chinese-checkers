cmake_minimum_required(VERSION 3.15)

project(ChineseCheckers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(spdlog REQUIRED)

add_definitions(-DHAVE_SPDLOG)

add_executable(chinesecheckers_server src/server.cpp src/httplib.h src/game.cpp src/game.hpp src/table.cpp src/table.hpp)
target_link_libraries(chinesecheckers_server spdlog::spdlog)
target_include_directories(chinesecheckers_server PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(chinesecheckers_cli src/cli.cpp src/game.cpp src/game.hpp src/table.cpp src/table.hpp)
target_link_libraries(chinesecheckers_cli spdlog::spdlog)
target_include_directories(chinesecheckers_cli PRIVATE ${CMAKE_SOURCE_DIR}/src)


include(ExternalProject)
ExternalProject_Add(
    chinesecheckers_wasm
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/wasm
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/web
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EMCC_TOOCHAIN_FILE} -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/web
    BUILD_ALWAYS true
)
