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

#ifndef KSMOOTHDOCK_DESKTOP_SELECTOR_H_
#define KSMOOTHDOCK_DESKTOP_SELECTOR_H_

#include "icon_based_dock_item.h"

#include <QFile>
#include <QMenu>
#include <QObject>
#include <QString>

#include <KConfig>
#include <KWindowSystem>

namespace ksmoothdock {

class DesktopSelector : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  DesktopSelector(KSmoothDock* parent, Qt::Orientation orientation, int minSize,
      int maxSize, int desktop, KConfig *config);

  virtual ~DesktopSelector() {}

  virtual void draw(QPainter* painter) const override;

  virtual void mousePressEvent(QMouseEvent* e) override;

 public slots:
   void changeWallpaper();

   void updateWallpaper(int currentDesktop);

 private:
  static void setWallpaper(const QString& wallpaper);

  // Sets the icon but scales the pixmap to the screen's width/height ratio.
  void setIconScaled(const QPixmap& icon);

  bool isCurrentDesktop() const {
    return KWindowSystem::currentDesktop() == desktop_;
  }

  QString getConfigKey() const {
    return QString("wallpaper") + QString::number(desktop_);
  }

  bool isWallpaperOk() const {
    return !wallpaper_.isEmpty() && QFile::exists(wallpaper_);
  }

  void createMenu();

  // The desktop that this desktop selector manages, 1-based.
  int desktop_;
  KConfig* config_;

  // The path to the wallpaper image file.
  QString wallpaper_;

  // Context (right-click) menu.
  QMenu menu_;

  int desktopWidth_;
  int desktopHeight_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DESKTOP_SELECTOR_H_
