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

#include "wallpaper_utils.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace ksmoothdock {

QString loadPlasmaWallpaper(const QString& plasmaConfig) {
  static QString wallpaper("");

  if (!wallpaper.isEmpty()) {
    return wallpaper;
  }

  QFile config(plasmaConfig);
  if (!config.open(QFile::ReadOnly)) {
    return wallpaper;
  }

  static const QString kConfigGroupPart1 = QString("[Containments][");
  static const QString kConfigGroupPart2 = QString(
      "][Wallpaper][org.kde.image][General]");
  QTextStream input(&config);
  QString line;
  bool foundConfigGroup = false;
  while (input.readLineInto(&line)) {
    if (!foundConfigGroup) {
      if (line.startsWith(kConfigGroupPart1) &&
          line.mid(16, 36) == kConfigGroupPart2) {
        foundConfigGroup = true;
      }
    } else {
      if (line.startsWith("Image=file://")) {
        wallpaper = line.mid(13);
        break;
      }
    }
  }

  return wallpaper;
}

}  // namespace ksmoothdock
