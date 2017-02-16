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

#include "launcher.h"

#include <QProcess>

namespace ksmoothdock {

Launcher::Launcher(KSmoothDock* parent, int itemId, QString label, 
    Qt::Orientation orientation, QString iconName, int minSize,
    int maxSize, QString command)
    : IconBasedDockItem(parent, itemId, label, orientation, iconName, minSize, 
      maxSize), command_(command) {}

void Launcher::mousePressEvent(QMouseEvent* e) const {
  if (e->button() == Qt::LeftButton) { // Run the application.
    QProcess::startDetached(command_);
  }
}

}  // namespace ksmoothdock
