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

#include <QFile>
#include <QIODevice>
#include <QProcess>
#include <QTextStream>

#include <KDesktopFile>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowSystem>

namespace ksmoothdock {

Launcher::Launcher(const QString& label, Qt::Orientation orientation,
    const QString& iconName, int minSize, int maxSize, const QString& command)
    : IconBasedDockItem(label, orientation, iconName, minSize, maxSize), 
      iconName_(iconName), command_(command) {}

Launcher::Launcher(const QString& file, Qt::Orientation orientation,
    int minSize, int maxSize)
    : IconBasedDockItem("", orientation, "", minSize, maxSize) {
  KDesktopFile desktopFile(file);
  label_ = desktopFile.readName();
  command_ = desktopFile.entryMap("Desktop Entry")["Exec"];
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
  QFile out(filePath);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
    std::cerr << "Failed to write to file " << filePath.toStdString()
        << std::endl;
  }

  QTextStream out_s(&out);
  out_s << "[Desktop Entry]\n";
  out_s << "Comment=\n";
  out_s << "Exec=" << command_ << endl;
  out_s << "GenericName=\n";
  out_s << "Icon=" << iconName_ << endl;
  out_s << "MimeType=\n";
  out_s << "Name=" << label_ << endl;
  out_s << "Path=\n";
  out_s << "StartupNotify=true\n";
  out_s << "Terminal=false\n";
  out_s << "TerminalOptions=\n";
  out_s << "Type=Application\n";
  out_s << "X-DBUS-ServiceName=\n";
  out_s << "X-DBUS-StartupType=\n";
  out_s << "X-KDE-SubstituteUID=false\n";
  out_s << "X-KDE-Username=\n";

  out.close();
}

void Launcher::launch(const QString& command) {
  if (!QProcess::startDetached(command)) {
    KMessageBox::error(nullptr,
        i18n("Could not run command: ") + command);
  }
}

}  // namespace ksmoothdock
