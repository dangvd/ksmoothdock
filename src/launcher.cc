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

#include "launcher.h"

#include <iostream>

#include <QProcess>

#include <KConfig>
#include <KConfigGroup>
#include <KDesktopFile>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowSystem>

namespace ksmoothdock {

Launcher::Launcher(DockPanel* parent, const QString& label,
    Qt::Orientation orientation, const QString& iconName, int minSize,
    int maxSize, const QString& command)
    : IconBasedDockItem(parent, label, orientation, iconName, minSize,
          maxSize), 
      command_(command) {}

void Launcher::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) { // Run the application.
    if (command_ == kShowDesktopCommand) {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    } else {
      launch(command_);
    }
  }
}

void Launcher::launch(const QString& command) {
  if (!QProcess::startDetached(command)) {
    KMessageBox::error(nullptr,
        i18n("Could not run command: ") + command);
  }
}

}  // namespace ksmoothdock
