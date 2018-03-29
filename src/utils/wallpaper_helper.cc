/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2018 Viet Dang (dangvd@gmail.com)
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

#include "wallpaper_helper.h"

#include <QApplication>
#include <QDBusMessage>
#include <QDesktopWidget>
#include <QFile>

#include <KMessageBox>

namespace ksmoothdock {

WallpaperHelper::WallpaperHelper(MultiDockModel* model)
    : model_(model),
      plasmaShellDBus_("org.kde.plasmashell",
                       "/PlasmaShell",
                       "org.kde.PlasmaShell") {}

void WallpaperHelper::setPlasmaWallpapers(int desktop) {
  if (!plasmaShellDBus_.isValid()) {  // Not running in KDE Plasma 5.
    return;
  }

  if (!model_->hasPager()) {
    return;
  }

  for (int screen = 0; screen < QApplication::desktop()->screenCount();
       ++screen) {
    const auto& wallpaper = model_->wallpaper(desktop, screen);
    if (wallpaper.isEmpty()) {
      continue;
    }

    if (!QFile::exists(wallpaper)) {
      KMessageBox::error(nullptr,
                         i18n("Failed to load wallpaper from: ") + wallpaper);
      return;
    }

    const QDBusMessage response = plasmaShellDBus_.call(
        "evaluateScript",
        "var allDesktops = desktops();"
        "d = allDesktops[" + QString::number(screen) + "];" +
        "d.wallpaperPlugin ='org.kde.image';"
        "d.currentConfigGroup = Array('Wallpaper', 'org.kde.image','General');"
        "d.writeConfig('Image','file://"
        + wallpaper + "')");
    if (response.type() == QDBusMessage::ErrorMessage) {
      KMessageBox::error(
          nullptr,
          i18n("Failed to update wallpaper. Please make sure Plasma desktop "
               "widgets are unlocked in order to set wallpaper."));
      return;
    }
  }
}

}  // namespace ksmoothdock
