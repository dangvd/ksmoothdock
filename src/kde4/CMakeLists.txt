cmake_minimum_required(VERSION 2.8.11)

project(ksmoothdock)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_executable(ksmoothdock ../main.cc ../icon_based_dock_item.cc
    ../ksmoothdock.cc ../launcher.cc ../show_desktop.cc ../tooltip.cc)

target_link_libraries(ksmoothdock ${KDE4_KDEUI_LIBS})

# install(TARGETS ksmoothdock RUNTIME DESTINATION bin)
