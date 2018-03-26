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

#ifndef KSMOOTHDOCK_FONT_UTILS_H_
#define KSMOOTHDOCK_FONT_UTILS_H_

#include <algorithm>

#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <QString>

namespace ksmoothdock {

// Returns a QFont with font size adjusted automatically according to the given
// width, height, reference string and scale factor.
inline QFont adjustFontSize(int w, int h, const QString& referenceString,
                            float scaleFactor) {
  QFont font;
  QFontMetrics metrics(font);
  const QRect& rect = metrics.tightBoundingRect(referenceString);
  // Scale the font size according to the size of the dock.
  font.setPointSize(std::min(font.pointSize() * w / rect.width(),
                             font.pointSize() * h / rect.height()));
  font.setPointSize(static_cast<int>(font.pointSize() * scaleFactor));

  return font;
}

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_FONT_UTILS_H_
