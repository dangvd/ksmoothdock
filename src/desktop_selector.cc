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

#include <QApplication>
#include <QDBusMessage>
#include <QDesktopWidget>
#include <QDir>
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
      config_(config),
      plasmaShellDBus_("org.kde.plasmashell",
                       "/PlasmaShell",
                       "org.kde.PlasmaShell"),
      desktopWidth_(QApplication::desktop()->screenGeometry().width()),
      desktopHeight_(QApplication::desktop()->screenGeometry().height()) {}

void DesktopSelector::init() {
  KConfigGroup group(config_, "Pager");
  wallpaper_ = group.readEntry(getConfigKey(), "");
  if (wallpaper_.isEmpty() && desktop_ == 1) {
    KMessageBox::information(
        Q_NULLPTR,
        i18n("It's recommended to use the pager's wallpaper setting "
            "functionality to better distinguish between different virtual "
            "desktops."),
        i18n("Information"),
        "showPagerInfo");
  }
  if (isWallpaperOk()) {
    setIconScaled(QPixmap(wallpaper_));
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

  // Only draw the border for the current desktop if using a custom wallpaper
  // and dock has border.
  if (isCurrentDesktop() && isWallpaperOk() && parent_->hasBorder()) {
    painter->setPen(parent_->getBorderColor());
    painter->drawRect(left_ - 1, top_ - 1, getWidth() + 1, getHeight() + 1);
  }
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

void DesktopSelector::setIconScaled(const QPixmap& icon) {
  if (icon.width() * desktopHeight_ != icon.height() * desktopWidth_) {
    QPixmap scaledIcon = icon.scaled(desktopWidth_, desktopHeight_);
    setIcon(scaledIcon);
  } else {
    setIcon(icon);
  }
}

void DesktopSelector::changeWallpaper() {
  KMessageBox::information(
      Q_NULLPTR,
      i18n("Setting wallpaper requires Plasma desktop widgets to stay "
          "unlocked."),
      i18n("Information"),
      "showWallpaperSettingInfo");
  const QString& wallpaper = QFileDialog::getOpenFileName(
      parent_,
      i18n("Select Wallpaper Image"),
      QDir::homePath(),
      i18n("Image Files (*.png *.jpg *.bmp)"));
  if (wallpaper.isEmpty()) {
    return;
  }

  wallpaper_ = wallpaper;
  setIconScaled(QPixmap(wallpaper_));
  parent_->refresh();

  if (isCurrentDesktop()) {
    setWallpaper(wallpaper_);
  }

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
  if (!plasmaShellDBus_.isValid()) {  // Not running in KDE Plasma 5.
    return;
  }

  if (wallpaper.isEmpty()) {
    return;
  }

  if (!QFile::exists(wallpaper)) {
    KMessageBox::error(
        Q_NULLPTR,
        i18n("Failed to load wallpaper from: ") + wallpaper);
    return;
  }

  const QDBusMessage response = plasmaShellDBus_.call(
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
            "widgets are unlocked in order to set wallpaper."));
  }
}

void DesktopSelector::createMenu() {
  menu_.addAction(
      i18n("Set Wallpaper For Desktop ") + QString::number(desktop_),
      this, 
      SLOT(changeWallpaper()));
}

}  // namespace ksmoothdock
