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
#include "show_desktop.h"

#include <QProcess>

namespace ksmoothdock {

Launcher::Launcher(KSmoothDock* parent, int itemId, QString label, 
    Qt::Orientation orientation, QString iconName, int minSize,
    int maxSize, QString command)
    : IconBasedDockItem(parent, itemId, label, orientation, iconName, minSize, 
      maxSize), command_(command), isLaunching_(false) {}

void Launcher::draw(QPainter* painter) const {
  if (isLaunching_) {
    // Draws an active (i.e. just clicked-on) quick launcher. The icon will have
    // some special effect to acknowledge launching the program.
    // TODO
    painter->drawPixmap(left_, top_, icons_[size_ - minSize_]);
  } else {
    IconBasedDockItem::draw(painter);
  }
}

void Launcher::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) { // Run the application.
    if (command_ == kShowDesktopCommand) {
      ShowDesktop::instance()->toggleShowDesktop();
    } else {
      QProcess::startDetached(command_);
      isLaunching_ = true;
    }
  }
}

}  // namespace ksmoothdock
