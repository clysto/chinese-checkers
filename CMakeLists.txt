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

set(APP_SOURCES
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
set(MACOSX_BUNDLE_ICON_FILE chinesecheckers.icns)
set(MACOSX_BUNDLE_ICON_PATH ${CMAKE_CURRENT_LIST_DIR}/misc/chinesecheckers.icns)
set_source_files_properties(${MACOSX_BUNDLE_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

if(APPLE)
    add_executable(chinesecheckers_gui MACOSX_BUNDLE ${MACOSX_BUNDLE_ICON_PATH} ${APP_SOURCES})
    target_link_libraries(chinesecheckers_gui fltk::fltk fltk::images spdlog::spdlog Freetype::Freetype)
elseif(WIN32)
    add_executable(chinesecheckers_gui ${APP_SOURCES} misc/chinesecheckers.rc)
    target_link_libraries(chinesecheckers_gui fltk::fltk fltk::images spdlog::spdlog Freetype::Freetype)
elseif(LINUX)
    set_target_properties(fltk::fltk PROPERTIES
        INTERFACE_LINK_LIBRARIES ""
    )
    add_executable(chinesecheckers_gui ${APP_SOURCES})
    target_link_libraries(chinesecheckers_gui
        fltk::fltk
        fltk::images
        spdlog::spdlog
        /opt/pango-1.52.1/lib/x86_64-linux-gnu/libpangocairo-1.0.a
        /opt/pango-1.52.1/lib/x86_64-linux-gnu/libpangoft2-1.0.a
        /opt/pango-1.52.1/lib/x86_64-linux-gnu/libpango-1.0.a
        /usr/lib/x86_64-linux-gnu/libcairo.so
        /usr/lib/x86_64-linux-gnu/libdl.a
        /usr/lib/x86_64-linux-gnu/libSM.so
        /usr/lib/x86_64-linux-gnu/libICE.so
        /usr/lib/x86_64-linux-gnu/libX11.so
        /usr/lib/x86_64-linux-gnu/libXext.so
        /usr/lib/x86_64-linux-gnu/libXrender.so
        /usr/lib/x86_64-linux-gnu/libfreetype.so
        /usr/lib/x86_64-linux-gnu/libcairo.so
        /usr/lib/x86_64-linux-gnu/libgobject-2.0.so
        /usr/lib/x86_64-linux-gnu/libgio-2.0.so
        /usr/lib/x86_64-linux-gnu/libglib-2.0.so
        /usr/lib/x86_64-linux-gnu/libharfbuzz.so
        /usr/lib/x86_64-linux-gnu/libfribidi.so
        /usr/lib/x86_64-linux-gnu/libthai.so
        /usr/lib/x86_64-linux-gnu/libXft.so
        /usr/lib/x86_64-linux-gnu/libfontconfig.so
    )
endif()

target_compile_definitions(chinesecheckers_gui PUBLIC -DHAVE_SPDLOG)
target_include_directories(chinesecheckers_gui PRIVATE ${CMAKE_SOURCE_DIR}/src)

set_target_properties(chinesecheckers_gui PROPERTIES
    MACOSX_BUNDLE on
    WIN32_EXECUTABLE on
    OUTPUT_NAME "Chinese Checkers"
)

include(ExternalProject)
ExternalProject_Add(
    chinesecheckers_wasm
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/wasm
    INSTALL_DIR ${CMAKE_SOURCE_DIR}/web
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${EMCC_TOOCHAIN_FILE} -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/web
    BUILD_ALWAYS true
)
