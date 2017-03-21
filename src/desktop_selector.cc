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
#include <QDBusMessage>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>

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
  wallpaper_ = group.readEntry(getConfigKey(), "");
  if (!wallpaper_.isEmpty() && QFile::exists(wallpaper_)) {
    setIcon(QPixmap(wallpaper_));
  } else {
    setIconName("user-desktop");
  }
  createMenu();
  connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
      this, SLOT(updateWallpaper(int)));
  updateWallpaper(KWindowSystem::currentDesktop());
}

void DesktopSelector::draw(QPainter* painter) const {
  IconBasedDockItem::draw(painter);

  QColor borderColor;
  if (isCurrentDesktop()) {
    KConfigGroup group(config_, "Pager");
    borderColor = group.readEntry("currentDesktopBorderColor",
        QColor("#ffff00"));
  } else {
    KConfigGroup group(config_, "General");
    // TODO(dangvd): This is the same default value as that in ksmoothdock.cc
    // so they should be moved to some common place.
    borderColor = group.readEntry("borderColor", QColor("#b1c4de"));
  }
  painter->setPen(borderColor);
  painter->drawRect(left_, top_, getWidth(), getHeight());
}

void DesktopSelector::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    if (isCurrentDesktop()) {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    } else {
      KWindowSystem::setCurrentDesktop(desktop_);
    }
  } else if (e->button() == Qt::RightButton) {
    menu_.popup(e->globalPos());
  }
}

void DesktopSelector::changeWallpaper() {
  KMessageBox::information(
      parent_,
      i18n("Setting wallpaper requires Plasma desktop widgets to stay "
          "unlocked."),
      i18n("Information"),
      "showWallpaperSettingInfo");
  QString wallpaper = QFileDialog::getOpenFileName(
      parent_,
      i18n("Select Wallpaper Image"),
      QDir::homePath(),
      i18n("Image Files (*.png *.jpg *.bmp)"));
  if (wallpaper.isEmpty()) {
    return;
  }

  wallpaper_ = wallpaper;
  setIcon(QPixmap(wallpaper_));
  parent_->refresh();

  setWallpaper(wallpaper_);

  KConfigGroup group(config_, "Pager");
  group.writeEntry(getConfigKey(), wallpaper_);
  config_->sync();
}

void DesktopSelector::updateWallpaper(int currentDesktop) {
  if (currentDesktop == desktop_) {
    setWallpaper(wallpaper_);
    parent_->update();
  }
}

void DesktopSelector::setWallpaper(const QString& wallpaper) {
  if (wallpaper.isEmpty()) {
    return;
  }

  if (!QFile::exists(wallpaper)) {
    KMessageBox::error(
        Q_NULLPTR,
        i18n("Failed to load wallpaper from: ") + wallpaper);
    return;
  }

  QDBusInterface plasmaShell(
      "org.kde.plasmashell",
      "/PlasmaShell",
      "org.kde.PlasmaShell");
  const QDBusMessage& response = plasmaShell.call(
      "evaluateScript",
      "var allDesktops = desktops();"
      "for (i=0;i<allDesktops.length;i++) {"
      "d = allDesktops[i];"
      "d.wallpaperPlugin ='org.kde.image';"
      "d.currentConfigGroup = Array('Wallpaper', 'org.kde.image','General');"
      "d.writeConfig('Image','file://"
      + wallpaper + "')}");
  if (response.type() == QDBusMessage::ErrorMessage) {
    KMessageBox::error(
        Q_NULLPTR,
        i18n("Failed to update wallpaper. Please make sure Plasma desktop "
            "widgets are unlocked and try again."));
  }
}

void DesktopSelector::createMenu() {
  menu_.addAction(
      i18n("Set Wallpaper For Desktop ") + QString::number(desktop_),
      this, 
      SLOT(changeWallpaper()));
}

}  // namespace ksmoothdock
