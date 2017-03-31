cmake_minimum_required(VERSION 2.8.11)

project(ksmoothdock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror=return-type")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(KF5Config REQUIRED)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5DBusAddons REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(KF5IconThemes REQUIRED)
find_package(KF5XmlGui REQUIRED)
find_package(KF5WidgetsAddons REQUIRED)
find_package(KF5WindowSystem REQUIRED)

add_executable(ksmoothdock main.cc icon_based_dock_item.cc ksmoothdock.cc
    launcher.cc tooltip.cc config_dialog.cc welcome_dialog.cc
    desktop_selector.cc edit_launchers_dialog.cc)

target_link_libraries(ksmoothdock Qt5::DBus)
target_link_libraries(ksmoothdock Qt5::Gui)
target_link_libraries(ksmoothdock Qt5::Widgets)
target_link_libraries(ksmoothdock KF5::ConfigCore KF5::ConfigGui)
target_link_libraries(ksmoothdock KF5::CoreAddons)
target_link_libraries(ksmoothdock KF5::DBusAddons)
target_link_libraries(ksmoothdock KF5::I18n)
target_link_libraries(ksmoothdock KF5::IconThemes)
target_link_libraries(ksmoothdock KF5::XmlGui)
target_link_libraries(ksmoothdock KF5::WidgetsAddons)
target_link_libraries(ksmoothdock KF5::WindowSystem)

install(TARGETS ksmoothdock RUNTIME DESTINATION bin)
install(FILES ksd.ksmoothdock.desktop DESTINATION /usr/share/applications)
