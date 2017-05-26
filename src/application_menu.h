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

#include <vector>

#include <QMenu>

#include <KConfig>

namespace ksmoothdock {

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
};

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
  std::vector<ApplicationEntry> entries;

  Category(const QString& name2, const QString& displayName2,
           const QString& icon2)
      : name(name2), displayName(displayName2), icon(icon2) {}
};

// The application menu, i.e. a cascading popup menu that contains entries
// for all applications organized by categories.
//
// It uses a custom style e.g. bigger icon size and spacing.
class ApplicationMenu : public IconBasedDockItem {
 public:
  ApplicationMenu(KSmoothDock* parent, Qt::Orientation orientation, int minSize,
                  int maxSize, KConfig* config,
                  const QString& entryDir = "/usr/share/applications");

  void mousePressEvent(QMouseEvent* e) override;

 private:
  void loadConfig();

  // Initializes application categories.
  void initCategories();

  // Loads application entries.
  void loadEntries();

  // Builds the menu from the application entries;
  void buildMenu();

  KConfig* config_;

  // The directory that contains the list of all application entries as desktop
  // files, e.g. /usr/share/applications
  QString entryDir_;

  // Application entries, organized on categories.
  std::vector<Category> categories_;

  // The cascading popup menu that contains all application entries.
  QMenu menu_;
};

}  // namespace ksmoothdock

#endif // KSMOOTHDOCK_APPLICATION_MENU_H_
