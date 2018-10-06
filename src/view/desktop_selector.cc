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
#include <QBrush>
#include <QColor>
#include <QDesktopWidget>
#include <QFile>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

#include <KLocalizedString>
#include <KWindowSystem>

#include "dock_panel.h"
#include <utils/draw_utils.h>
#include <utils/font_utils.h>

namespace ksmoothdock {

DesktopSelector::DesktopSelector(DockPanel* parent, MultiDockModel* model,
                                 Qt::Orientation orientation, int minSize,
                                 int maxSize, int desktop, int screen)
    : IconBasedDockItem(parent, 
          i18n("Desktop ") + QString::number(desktop),
          orientation, "" /* no icon yet */, minSize, maxSize),
      model_(model),
      desktop_(desktop),
      screen_(screen),
      desktopWidth_(QApplication::desktop()->screenGeometry().width()),
      desktopHeight_(QApplication::desktop()->screenGeometry().height()),
      hasCustomWallpaper_(false) {
  createMenu();
  loadConfig();
}

void DesktopSelector::draw(QPainter* painter) const {
  if (hasCustomWallpaper_) {
    IconBasedDockItem::draw(painter);
  } else {
    // Draw rectangles with desktop numbers if no custom wallpapers set.
    QColor fillColor = model_->backgroundColor().lighter();
    fillColor.setAlphaF(0.42);
    painter->fillRect(left_, top_, getWidth(), getHeight(), QBrush(fillColor));
  }

  if (model_->showDesktopNumber()) {
    painter->setFont(adjustFontSize(getWidth(), getHeight(),
                                    "0" /* reference string */,
                                    0.5 /* scale factor */));
    painter->setRenderHint(QPainter::TextAntialiasing);
    drawBorderedText(left_, top_, getWidth(), getHeight(), Qt::AlignCenter,
                     QString::number(desktop_), 1 /* borderWidth */, Qt::black,
                     Qt::white, painter);
  }

  // Draw the border for the current desktop.
  if (isCurrentDesktop()) {
    painter->setPen(parent_->borderColor());
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
    // In case other DesktopSelectors have changed the config.
    showDesktopNumberAction_->setChecked(model_->showDesktopNumber());
    menu_.popup(e->globalPos());
  }
}

void DesktopSelector::loadConfig() {
  const auto& wallpaper = model_->wallpaper(desktop_, screen_);
  if (!wallpaper.isEmpty() && QFile::exists(wallpaper)) {
    setIconScaled(QPixmap(wallpaper));
    hasCustomWallpaper_ = true;
  }

  showDesktopNumberAction_->setChecked(model_->showDesktopNumber());
}

void DesktopSelector::saveConfig() {
  model_->setShowDesktopNumber(showDesktopNumberAction_->isChecked());
  model_->saveAppearanceConfig(true /* repaintOnly */);
}

void DesktopSelector::setIconScaled(const QPixmap& icon) {
  if (icon.width() * desktopHeight_ != icon.height() * desktopWidth_) {
    QPixmap scaledIcon = icon.scaled(desktopWidth_, desktopHeight_);
    setIcon(scaledIcon);
  } else {
    setIcon(icon);
  }
}

void DesktopSelector::createMenu() {
  menu_.addAction(
      QIcon::fromTheme("preferences-desktop-wallpaper"),
      i18n("Set Wallpaper for Desktop ") + QString::number(desktop_),
      parent_,
      [this] {
        parent_->showWallpaperSettingsDialog(desktop_);
      });
  showDesktopNumberAction_ = menu_.addAction(
      i18n("Show Desktop Number"), this,
      [this] {
        saveConfig();
      });
  showDesktopNumberAction_->setCheckable(true);
}

}  // namespace ksmoothdock
