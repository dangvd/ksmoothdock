/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2015 Viet Dang (dangvd@gmail.com)
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

#ifndef KSMOOTHDOCK_LAUNCHER_H_
#define KSMOOTHDOCK_LAUNCHER_H_

#include "icon_based_dock_item.h"

namespace ksmoothdock {

static constexpr char kShowDesktopCommand[] = "SHOW_DESKTOP";

class Launcher : public IconBasedDockItem {
 public:

  Launcher(QString label, Qt::Orientation orientation,
      QString iconName, int minSize, int maxSize, QString command);

  // Loads the icon name, command and label from a file in desktop file format.
  Launcher(QString file, Qt::Orientation orientation, int minSize,
      int maxSize);

  virtual ~Launcher() {}

  bool isCommandInternal() { return command_ == kShowDesktopCommand; }

  virtual void draw(QPainter* painter) const override;

  virtual void mousePressEvent(QMouseEvent* e) override;

  // Saves to file in desktop file format.
  void saveToFile(QString filePath);

 private:
  QString iconName_;
  QString command_;
  bool isLaunching_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_LAUNCHER_H_
