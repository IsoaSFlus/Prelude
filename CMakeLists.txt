cmake_minimum_required(VERSION 3.0)
# SET(CMAKE_INSTALL_PREFIX /usr CACHE PATH "Set install prefix.")
project(Prelude)
set(PROJECT_VERSION "0.6.7")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM 1.0 REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ECMUninstallTarget)

set(CMAKE_MODULE_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Core Gui Widgets Network Svg Qml Quick QuickControls2 QuickWidgets)

install(FILES "misc/prelude.desktop"
	        DESTINATION "share/applications") 

add_subdirectory(src)
