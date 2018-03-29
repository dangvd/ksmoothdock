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

#ifndef KSMOOTHDOCK_WALLPAPER_HELPER_H_
#define KSMOOTHDOCK_WALLPAPER_HELPER_H_

#include <QDBusInterface>
#include <QObject>

#include <model/multi_dock_model.h>

namespace ksmoothdock {

// Helper class for working with desktop wallpapers.
class WallpaperHelper : public QObject {
  Q_OBJECT

 public:
  explicit WallpaperHelper(MultiDockModel* model);
  ~WallpaperHelper() = default;

 public slots:
  void setPlasmaWallpapers(int desktop);

 private:
  MultiDockModel* model_;

  QDBusInterface plasmaShellDBus_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_WALLPAPER_HELPER_H_
