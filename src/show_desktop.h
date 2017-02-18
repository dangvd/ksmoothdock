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

#ifndef KSMOOTHDOCK_SHOW_DESKTOP_H_
#define KSMOOTHDOCK_SHOW_DESKTOP_H_

#include <vector>

#include <QObject>

#include <KWindowSystem>

namespace ksmoothdock {

// Singleton class to show/hide the desktop.
class ShowDesktop : public QObject {
  Q_OBJECT

 public:
  static ShowDesktop* instance();

  // Shows/hides the desktop.
  void toggleShowDesktop();

 private:
  bool isShowingDesktop_;
  std::vector<WId> minimizedWindows_;
  WId activeWindow_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_SHOW_DESKTOP_H_
