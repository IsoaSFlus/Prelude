cmake_minimum_required(VERSION 3.0)
# SET(CMAKE_INSTALL_PREFIX /usr CACHE PATH "Set install prefix.")
project(Prelude)
set(PROJECT_VERSION "0.6.1")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


# find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Gui Widgets Network Svg Qml Quick QuickControls2 QuickWidgets)

add_subdirectory(src)
