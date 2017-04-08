cmake_minimum_required(VERSION 2.8.12)

project(ksmoothdock)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror=return-type")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS DBus Gui Test Widgets)
find_package(KF5 REQUIRED COMPONENTS Config CoreAddons DBusAddons I18n
    IconThemes XmlGui WidgetsAddons WindowSystem)

set(SRCS icon_based_dock_item.cc ksmoothdock.cc
  launcher.cc tooltip.cc config_dialog.cc welcome_dialog.cc
  desktop_selector.cc edit_launchers_dialog.cc)
add_executable(ksmoothdock main.cc ${SRCS})

set(LIBS Qt5::DBus Qt5::Gui Qt5::Widgets KF5::ConfigCore KF5::ConfigGui
    KF5::CoreAddons KF5::DBusAddons KF5::I18n KF5::IconThemes KF5::XmlGui
    KF5::WidgetsAddons KF5::WindowSystem)
target_link_libraries(ksmoothdock ${LIBS})

# Install

install(TARGETS ksmoothdock RUNTIME DESTINATION bin)
install(FILES ksd.ksmoothdock.desktop DESTINATION /usr/share/applications)

# Uninstall

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND}
    -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Test

enable_testing()

add_executable(config_dialog_test config_dialog_test.cc ${SRCS})
target_link_libraries(config_dialog_test Qt5::Test ${LIBS})
add_test(config_dialog_test config_dialog_test)

add_executable(desktop_selector_test desktop_selector_test.cc ${SRCS})
target_link_libraries(desktop_selector_test Qt5::Test ${LIBS})
add_test(desktop_selector_test desktop_selector_test)
