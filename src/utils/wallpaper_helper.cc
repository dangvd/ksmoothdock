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

#include <QDBusMessage>
#include <QFile>
#include <QGuiApplication>

#include <KMessageBox>
#include <KWindowSystem>

namespace ksmoothdock {

WallpaperHelper::WallpaperHelper(MultiDockModel* model)
    : model_(model),
      plasmaShellDBus_("org.kde.plasmashell",
                       "/PlasmaShell",
                       "org.kde.PlasmaShell") {}

void WallpaperHelper::setPlasmaWallpapers() {
  if (!plasmaShellDBus_.isValid()) {  // Not running in KDE Plasma 5.
    return;
  }

  if (!model_->hasPager()) {
    return;
  }

  for (int screen = 0; screen < QGuiApplication::screens().size();
       ++screen) {
    if (!doSetPlasmaWallpaper(screen)) {
      return;
    }
  }
}

void WallpaperHelper::setPlasmaWallpaper(int screen) {
  if (!plasmaShellDBus_.isValid()) {  // Not running in KDE Plasma 5.
    return;
  }

  if (!model_->hasPager()) {
    return;
  }

  doSetPlasmaWallpaper(screen);
}

bool WallpaperHelper::doSetPlasmaWallpaper(int screen) {
  const auto& wallpaper = model_->wallpaper(KWindowSystem::currentDesktop(),
                                            screen);
  if (wallpaper.isEmpty()) {
    return true;  // nothing to do here.
  }

  if (!QFile::exists(wallpaper)) {
    KMessageBox::error(nullptr,
                       i18n("Failed to load wallpaper from: ") + wallpaper);
    return false;
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
    return false;
  }

  return true;
}

}  // namespace ksmoothdock
