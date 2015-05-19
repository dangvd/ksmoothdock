cmake_minimum_required(VERSION 2.8.11)

project(ksmoothdock)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(KF5IconThemes REQUIRED)
find_package(KF5WindowSystem REQUIRED)

add_executable(ksmoothdock main.cc dockitem.cc dockmodel.cc 
    paraboliczoomview.cc)

target_link_libraries(ksmoothdock Qt5::Gui)
target_link_libraries(ksmoothdock Qt5::Widgets)
target_link_libraries(ksmoothdock KF5::IconThemes)
target_link_libraries(ksmoothdock KF5::WindowSystem)

# install(TARGETS ksmoothdock RUNTIME DESTINATION bin)
