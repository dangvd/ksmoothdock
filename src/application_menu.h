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

#include <QMenu>
#include <QPoint>

namespace ksmoothdock {

// The application menu, i.e. a cascading popup menu that contains entries
// for all applications organized by categories.
//
// It uses a custom style e.g. bigger icon size and spacing.
//
// This class is a singleton class.
class ApplicationMenu {
 public:
  static ApplicationMenu* instance();

  // Shows/hides the menu.
  void toggleMenu(const QPoint& pos);

  ApplicationMenu(const ApplicationMenu&) = delete;
  ApplicationMenu& operator=(const ApplicationMenu&) = delete;

 private:
  ApplicationMenu();

  // Loads categories and applications.
  void load();

  // The directory that contains the list of all applications,
  // e.g. /usr/share/applications
  QString appDir_;

  QMenu menu_;
};

}  // namespace ksmoothdock

#endif // KSMOOTHDOCK_APPLICATION_MENU_H_
