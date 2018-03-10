# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = KSmoothDock

TEMPLATE = app
TARGET = ksmoothdock

HEADERS = \
   $$PWD/src/add_panel_dialog.h \
   $$PWD/src/appearance_settings_dialog.h \
   $$PWD/src/application_menu.h \
   $$PWD/src/application_menu_settings_dialog.h \
   $$PWD/src/calendar.h \
   $$PWD/src/clock.h \
   $$PWD/src/command_utils.h \
   $$PWD/src/config_helper.h \
   $$PWD/src/desktop_selector.h \
   $$PWD/src/dock_item.h \
   $$PWD/src/dock_panel.h \
   $$PWD/src/edit_launchers_dialog.h \
   $$PWD/src/icon_based_dock_item.h \
   $$PWD/src/iconless_dock_item.h \
   $$PWD/src/launcher.h \
   $$PWD/src/multi_dock_model.h \
   $$PWD/src/multi_dock_view.h \
   $$PWD/src/tooltip.h \
   $$PWD/src/welcome_dialog.h

SOURCES = \
   $$PWD/src/add_panel_dialog.cc \
   $$PWD/src/appearance_settings_dialog.cc \
   $$PWD/src/application_menu.cc \
   $$PWD/src/application_menu_settings_dialog.cc \
   $$PWD/src/calendar.cc \
   $$PWD/src/clock.cc \
   $$PWD/src/config_helper.cc \
   $$PWD/src/desktop_selector.cc \
   $$PWD/src/dock_panel.cc \
   $$PWD/src/edit_launchers_dialog.cc \
   $$PWD/src/icon_based_dock_item.cc \
   $$PWD/src/iconless_dock_item.cc \
   $$PWD/src/launcher.cc \
   $$PWD/src/main.cc \
   $$PWD/src/multi_dock_model.cc \
   $$PWD/src/multi_dock_view.cc \
   $$PWD/src/tooltip.cc \
   $$PWD/src/welcome_dialog.cc \

FORMS = \
   $$PWD/src/add_panel_dialog.ui \
   $$PWD/src/appearance_settings_dialog.ui \
   $$PWD/src/application_menu_settings_dialog.ui \
   $$PWD/src/edit_launchers_dialog.ui \
   $$PWD/src/welcome_dialog.ui \

INCLUDEPATH = \
    $$PWD/. \
    $$PWD/src

QT += core gui widgets testlib KConfigCore KConfigGui \
    KCoreAddons KDBusAddons KI18n KIconThemes KXmlGui \
    KWidgetsAddons KWindowSystem

CONFIG += c++14

#DEFINES = 

