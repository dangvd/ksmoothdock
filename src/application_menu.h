/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2017 Viet Dang (dangvd@gmail.com)
 *
 * KSmoothDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KSmoothDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KSmoothDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KSMOOTHDOCK_APPLICATION_MENU_H_
#define KSMOOTHDOCK_APPLICATION_MENU_H_

#include "icon_based_dock_item.h"
#include "multi_dock_model.h"

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <QDir>
#include <QEvent>
#include <QFileSystemWatcher>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QProxyStyle>
#include <QSize>
#include <QString>
#include <QStringList>

#include "application_menu_settings_dialog.h"

namespace ksmoothdock {

constexpr int kApplicationMenuIconSize = 32;

class ApplicationMenuStyle : public QProxyStyle {
 public:
  int pixelMetric(PixelMetric metric, const QStyleOption *option = Q_NULLPTR,
                  const QWidget *widget = Q_NULLPTR) const override;
};

// An application entry in the application menu.
struct ApplicationEntry {
  // Name e.g. 'Chrome'.
  QString name;

  // Generic name e.g. 'Web Brower'.
  QString genericName;

  // Icon name e.g. 'chrome'.
  QString icon;

  // Command to execute e.g. 'chrome'.
  QString command;

  // The path to the desktop file e.g. '/usr/share/applications/chrome.desktop'
  QString desktopFile;

  ApplicationEntry(const QString& name2, const QString& genericName2,
                   const QString& icon2, const QString& command2,
                   const QString& desktopFile2)
      : name(name2), genericName(genericName2), icon(icon2), command(command2),
        desktopFile(desktopFile2) {}
};

bool operator<(const ApplicationEntry &e1, const ApplicationEntry &e2);

// A category in the application menu.
struct Category {
  // Name for the category e.g. 'Development' or 'Utility'. See:
  // https://specifications.freedesktop.org/menu-spec/latest/apa.html
  QString name;

  // Display name for the category e.g. 'Utilities'.
  QString displayName;

  // Icon name for the category e.g. 'applications-internet'.
  QString icon;

  // Application entries for this category.
  std::list<ApplicationEntry> entries;

  Category(const QString& name2, const QString& displayName2,
           const QString& icon2)
      : name(name2), displayName(displayName2), icon(icon2) {}

  Category(const QString& name2, const QString& displayName2,
           const QString& icon2, std::list<ApplicationEntry> entries2)
      : name(name2), displayName(displayName2), icon(icon2), entries(entries2) {
  }
};

// The application menu item on the dock.
//
// Left-clicking the item shows a cascading popup menu that contains entries
// for all applications organized by categories. The menu uses a custom style
// e.g. bigger icon size and the same translucent effect as the dock's.
//
// Supports drag-and-drop as a drag source.
// What it means is that you can drag an application entry from the menu
// to other widgets/applications. It doesn't support drag-and-drop within the
// menu itself.
class ApplicationMenu : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  ApplicationMenu(
      DockPanel* parent,
      MultiDockModel* model,
      Qt::Orientation orientation,
      int minSize,
      int maxSize,
      const QStringList& entryDirs = {
          "/usr/share/applications",
          "/usr/share/applications/kde4",
          QDir::homePath() + "/.local/share/applications"});
  virtual ~ApplicationMenu() {}

  void init();

  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;

  QSize getMenuSize() { return menu_.sizeHint(); }

 public slots:
  void reloadMenu();

  void showSettingsDialog();

protected:
  // Intercepts sub-menus's show events to adjust their position to improve
  // visibility.
  bool eventFilter(QObject* object, QEvent* event) override;

 private:
  QString getStyleSheet();
  
  // Initializes application categories.
  void initCategories();

  // Loads application entries from entryDir.
  bool loadEntries();

  // Loads an application entry from the .desktop file.
  bool loadEntry(const QString& file);

  // Builds the menu from the application entries;
  void buildMenu();

  void addToMenu(const std::vector<Category>& categories);

  void addEntry(const ApplicationEntry& entry, QMenu* menu);

  QIcon loadIcon(const QString& icon);

  static const std::vector<Category> kSessionSystemCategories;
  static const ApplicationEntry kSearchEntry;

  MultiDockModel* model_;

  // The directories that contains the list of all application entries as
  // desktop files, e.g. /usr/share/applications
  const QStringList entryDirs_;

  // Application entries, organized by categories.
  std::vector<Category> categories_;
  // Map from category names to category indices in the above vector,
  // to make loading entries faster.
  std::unordered_map<std::string, int> categoryMap_;

  // The cascading popup menu that contains all application entries.
  QMenu menu_;

  ApplicationMenuStyle style_;

  QFileSystemWatcher fileWatcher_;

  ApplicationMenuSettingsDialog settingsDialog_;

  // Drag support.

  // Starting mouse position, used for minimum drag distance check.
  QPoint startMousePos_;
  // The desktop file associated with the application entry being dragged.
  QString draggedEntry_;

  friend class ApplicationMenuTest;
};

}  // namespace ksmoothdock

#endif // KSMOOTHDOCK_APPLICATION_MENU_H_
