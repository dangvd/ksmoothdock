cmake_minimum_required(VERSION 3.16)

project(ksmoothdock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Werror)
add_definitions(-DQT_DEPRECATED_WARNINGS)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 5.11 REQUIRED COMPONENTS DBus Gui Test Widgets)
find_package(KF5 5.7 REQUIRED COMPONENTS Activities Config CoreAddons DBusAddons I18n
    IconThemes XmlGui WidgetsAddons WindowSystem)

set(SRCS
    model/application_menu_config.cc
    model/config_helper.cc
    model/multi_dock_model.cc
    view/add_panel_dialog.cc
    view/appearance_settings_dialog.cc
    view/application_menu_settings_dialog.cc
    view/application_menu.cc
    view/calendar.cc
    view/clock.cc
    view/desktop_selector.cc
    view/dock_panel.cc
    view/edit_launchers_dialog.cc
    view/icon_based_dock_item.cc
    view/iconless_dock_item.cc
    view/multi_dock_view.cc
    view/program.cc
    view/separator.cc
    view/task_manager_settings_dialog.cc
    view/tooltip.cc
    view/wallpaper_settings_dialog.cc
    utils/task_helper.cc
    utils/wallpaper_helper.cc)
add_library(ksmoothdock_lib ${SRCS})

set(LIBS Qt5::DBus Qt5::Gui Qt5::Widgets KF5::Activities KF5::ConfigCore KF5::ConfigGui
    KF5::CoreAddons KF5::DBusAddons KF5::I18n KF5::IconThemes KF5::XmlGui
    KF5::WidgetsAddons KF5::WindowSystem stdc++fs)
target_link_libraries(ksmoothdock_lib ${LIBS})

add_executable(ksmoothdock main.cc)
target_link_libraries(ksmoothdock ksmoothdock_lib ${LIBS})

# Install

install(TARGETS ksmoothdock RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ksd.ksmoothdock.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

# Uninstall

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND}
    -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# Test

enable_testing()

add_executable(appearance_settings_dialog_test
    view/appearance_settings_dialog_test.cc)
target_link_libraries(appearance_settings_dialog_test
    Qt5::Test ksmoothdock_lib ${LIBS})
add_test(appearance_settings_dialog_test appearance_settings_dialog_test)

add_executable(desktop_selector_test view/desktop_selector_test.cc)
target_link_libraries(desktop_selector_test Qt5::Test ksmoothdock_lib ${LIBS})
add_test(desktop_selector_test desktop_selector_test)

add_executable(edit_launchers_dialog_test view/edit_launchers_dialog_test.cc)
target_link_libraries(edit_launchers_dialog_test Qt5::Test ksmoothdock_lib
    ${LIBS})
add_test(edit_launchers_dialog_test edit_launchers_dialog_test)

add_executable(dock_panel_test view/dock_panel_test.cc)
target_link_libraries(dock_panel_test Qt5::Test ksmoothdock_lib ${LIBS})
add_test(dock_panel_test dock_panel_test)

add_executable(application_menu_config_test model/application_menu_config_test.cc)
target_link_libraries(application_menu_config_test Qt5::Test ksmoothdock_lib ${LIBS})
add_test(application_menu_config_test application_menu_config_test)

add_executable(add_panel_dialog_test view/add_panel_dialog_test.cc)
target_link_libraries(add_panel_dialog_test Qt5::Test ksmoothdock_lib ${LIBS})
add_test(add_panel_dialog_test add_panel_dialog_test)

add_executable(application_menu_settings_dialog_test
    view/application_menu_settings_dialog_test.cc)
target_link_libraries(application_menu_settings_dialog_test
    Qt5::Test ksmoothdock_lib ${LIBS})
add_test(application_menu_settings_dialog_test
    application_menu_settings_dialog_test)

add_executable(multi_dock_model_test model/multi_dock_model_test.cc)
target_link_libraries(multi_dock_model_test Qt5::Test ksmoothdock_lib ${LIBS})
add_test(multi_dock_model_test multi_dock_model_test)
