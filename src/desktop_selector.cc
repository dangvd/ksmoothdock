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

#include "desktop_selector.h"

#include <QDBusInterface>
#include <QDir>
#include <QFileDialog>
#include <QPixmap>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowSystem>

#include "ksmoothdock.h"

namespace ksmoothdock {

DesktopSelector::DesktopSelector(KSmoothDock* parent,
    Qt::Orientation orientation, int minSize,
    int maxSize, int desktop, KConfig *config)
    : IconBasedDockItem(parent, 
          i18n("Desktop ") + QString::number(desktop),
          orientation, "" /* no icon yet */, minSize, maxSize),
      desktop_(desktop),
      config_(config) {
  KConfigGroup group(config_, "Pager");
  QString wallpaper = group.readEntry(getConfigKey(), "");
  if (!wallpaper.isEmpty()) {
    setIcon(QPixmap(wallpaper));
  } else {
    setIconName("user-desktop");
  }
  createMenu();
}

void DesktopSelector::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    if (KWindowSystem::currentDesktop() != desktop_) {
      KWindowSystem::setCurrentDesktop(desktop_);
    } else {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    }
  } else if (e->button() == Qt::RightButton) {
    menu_.popup(e->globalPos());
  }
}

void DesktopSelector::changeWallpaper() {
  KMessageBox::information(
      parent_,
      i18n("Setting wallpaper requires Plasma desktop widgets to stay "
          "unlocked."));
  QString wallpaper = QFileDialog::getOpenFileName(
      parent_,
      i18n("Select Wallpaper Image"),
      QDir::homePath(),
      i18n("Image Files (*.png *.jpg *.bmp)"));
  setIcon(QPixmap(wallpaper));
  parent_->refresh();

  setWallpaper(wallpaper);

  KConfigGroup group(config_, "Pager");
  group.writeEntry(getConfigKey(), wallpaper);
  config_->sync();
}

void DesktopSelector::setWallpaper(const QString& wallpaper) {
  QDBusInterface plasmaShell(
      "org.kde.plasmashell",
      "/PlasmaShell",
      "org.kde.PlasmaShell");
  plasmaShell.call(
      "evaluateScript",
      "var allDesktops = desktops();"
      "for (i=0;i<allDesktops.length;i++) {"
      "d = allDesktops[i];"
      "d.wallpaperPlugin ='org.kde.image';"
      "d.currentConfigGroup = Array('Wallpaper', 'org.kde.image','General');"
      "d.writeConfig('Image','file://"
      + wallpaper + "')}");
}

void DesktopSelector::createMenu() {
  menu_.addAction(
      i18n("Set Wallpaper For Desktop ") + QString::number(desktop_),
      this, 
      SLOT(changeWallpaper()));
}

}  // namespace ksmoothdock
