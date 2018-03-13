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

#ifndef KSMOOTHDOCK_WALLPAPER_UTILS_H_
#define KSMOOTHDOCK_WALLPAPER_UTILS_H_

#include <QDir>
#include <QString>

namespace ksmoothdock {

// Returns the path to Plasma wallpaper's image file, given Plasma config file.
QString loadPlasmaWallpaper(const QString& plasmaConfig);

// Returns the path to Plasma wallpaper's image file.
inline QString loadPlasmaWallpaper() {
  return loadPlasmaWallpaper(
      QDir::homePath() + "/.config/plasma-org.kde.plasma.desktop-appletsrc");
}

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_WALLPAPER_UTILS_H_
