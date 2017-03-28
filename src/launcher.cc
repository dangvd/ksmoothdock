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

Launcher::Launcher(KSmoothDock* parent, const QString& label,
    Qt::Orientation orientation, const QString& iconName, int minSize,
    int maxSize, const QString& command)
    : IconBasedDockItem(parent, label, orientation, iconName, minSize,
          maxSize), 
      iconName_(iconName), command_(command) {}

Launcher::Launcher(KSmoothDock* parent, const QString& file,
    Qt::Orientation orientation, int minSize, int maxSize)
    : IconBasedDockItem(parent, "", orientation, "", minSize, maxSize) {
  KDesktopFile desktopFile(file);
  label_ = desktopFile.readName();
  command_ = filterFieldCodes(desktopFile.entryMap("Desktop Entry")["Exec"]);
  iconName_ = desktopFile.readIcon();
  setIconName(iconName_);
}

void Launcher::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) { // Run the application.
    if (command_ == kShowDesktopCommand) {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    } else {
      launch(command_);
    }
  }
}

void Launcher::saveToFile(const QString& filePath) {
  KConfig config(filePath, KConfig::SimpleConfig);
  KConfigGroup group(&config, "Desktop Entry");
  group.writeEntry("Name", label_);
  group.writeEntry("Exec", command_);
  group.writeEntry("Icon", iconName_);
  group.writeEntry("Type", "Application");
  group.writeEntry("Terminal", false);
  config.sync();
}

void Launcher::launch(const QString& command) {
  if (!QProcess::startDetached(command)) {
    KMessageBox::error(nullptr,
        i18n("Could not run command: ") + command);
  }
}

QString Launcher::filterFieldCodes(const QString& command) {
  if (command.contains('%')) {
    return command.left(command.indexOf(' '));
  }
  return command;
}

}  // namespace ksmoothdock
