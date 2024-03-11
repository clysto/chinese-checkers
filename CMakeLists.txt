cmake_minimum_required(VERSION 3.15)

project(ChineseCheckers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(spdlog REQUIRED)
find_package(FLTK CONFIG REQUIRED)

add_executable(chinesecheckers_server src/server.cpp src/httplib.h src/game.cpp src/game.hpp)
target_compile_definitions(chinesecheckers_server PUBLIC -DHAVE_SPDLOG)
target_link_libraries(chinesecheckers_server spdlog::spdlog)
target_include_directories(chinesecheckers_server PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(chinesecheckers_cli src/cli.cpp src/game.cpp src/game.hpp)
target_compile_definitions(chinesecheckers_cli PUBLIC -DHAVE_SPDLOG)
target_link_libraries(chinesecheckers_cli spdlog::spdlog)
target_include_directories(chinesecheckers_cli PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(chinesecheckers_gui
    src/gui/gui.cpp
    src/gui/utils.cpp
    src/gui/utils.hpp
    src/game.cpp
    src/game.hpp
    src/gui/widgets/Fl_IconButton.cpp
    src/gui/widgets/Fl_IconButton.hpp
    src/gui/widgets/Fl_ChessBoard.cpp
    src/gui/widgets/Fl_ChessBoard.hpp
)
target_compile_definitions(chinesecheckers_gui PUBLIC -DHAVE_SPDLOG)
target_link_libraries(chinesecheckers_gui fltk::fltk fltk::images spdlog::spdlog)
target_include_directories(chinesecheckers_gui PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(ExternalProject)
ExternalProject_Add(
    chinesecheckers_wasm
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/wasm
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/web
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EMCC_TOOCHAIN_FILE} -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/web
    BUILD_ALWAYS true
)
