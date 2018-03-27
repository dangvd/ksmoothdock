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

#include <memory>

#include <QDBusInterface>
#include <QFile>
#include <QMenu>
#include <QObject>
#include <QString>

#include <KWindowSystem>

#include <model/multi_dock_model.h>

namespace ksmoothdock {

// Pager icons.
class DesktopSelector : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  DesktopSelector(DockPanel* parent, MultiDockModel* model,
                  Qt::Orientation orientation, int minSize, int maxSize,
                  int desktop, int screen);

  void init();

  virtual ~DesktopSelector() {}

  int getWidthForSize(int size) const override {
    return isHorizontal() ? (size * desktopWidth_ / desktopHeight_) : size;
  }

  int getHeightForSize(int size) const override {
    return isHorizontal() ? size : (size * desktopHeight_ / desktopWidth_);
  }

  void draw(QPainter* painter) const override;
  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;

  // Sets the icon but scales the pixmap to the screen's width/height ratio.
  void setIconScaled(const QPixmap& icon);

 public slots:
  void updateWallpaper(int currentDesktop);

 private:
  bool isCurrentDesktop() const {
    return KWindowSystem::currentDesktop() == desktop_;
  }

  bool isWallpaperOk() const {
    return !wallpapers_[screen_].isEmpty() &&
        QFile::exists(wallpapers_[screen_]);
  }

  void setPlasmaWallpapers();

  void createMenu();

  MultiDockModel* model_;

  // The desktop that this desktop selector manages, 1-based.
  int desktop_;
  // The screen that the parent panel is on, 0-based.
  // We need this to show the correct image icon. However, we still need to
  // manage all wallpapers (for all screens) so that we can update the
  // desktop wallpapers when the current desktop has changed.
  int screen_;
  KConfig* config_;

  const int screenCount_;
  // The path to the wallpaper image files (one for each screen).
  std::unique_ptr<QString[]> wallpapers_;

  // Context (right-click) menu.
  QMenu menu_;

  QDBusInterface plasmaShellDBus_;

  int desktopWidth_;
  int desktopHeight_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DESKTOP_SELECTOR_H_
