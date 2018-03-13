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
  QFile config(plasmaConfig);
  if (!config.open(QFile::ReadOnly)) {
    return QString("");
  }

  static QRegularExpression regExp("\\[Containments\\]\\[\\d\\]\\[Wallpaper\\]"
                                   "\\[org.kde.image\\]\\[General\\]");
  QTextStream input(&config);
  QString line;
  bool foundConfigGroup = false;
  while (input.readLineInto(&line)) {
    if (!foundConfigGroup) {
      if (line.contains(regExp)) {
        foundConfigGroup = true;
      }
    } else {
      if (line.startsWith("Image=file://")) {
        return line.mid(13);
      }
    }
  }

  return QString("");
}

}  // namespace ksmoothdock
