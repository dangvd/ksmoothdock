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

#include "tooltip.h"

#include <QApplication>
#include <QPainter>

#include <KWindowSystem>
#include <netwm_def.h>

namespace ksmoothdock {

Tooltip::Tooltip() : QWidget(), font_(QApplication::font()) {
  setAttribute(Qt::WA_TranslucentBackground);
  KWindowSystem::setType(winId(), NET::Dock);
  KWindowSystem::setOnAllDesktops(winId(), true);
}

void Tooltip::setFontColor(const QColor& color) {
    fontColor_ = color;
}

void Tooltip::setBackgroundColor(const QColor& color) {
    backgroundColor_ = color;
}

void Tooltip::setFontFace(const QString& fontFace) {
    font_.setFamily(fontFace);
}

void Tooltip::setFontItalic(bool val) {
    font_.setItalic(val);
}

void Tooltip::setFontBold(bool val) {
    font_.setBold(val);
}

void Tooltip::setFontSize(int size) {
    fontSize_ = size;
    font_.setPointSize(size);
}

void Tooltip::setText(const QString& text) {
    text_ = text;
    updateLayout();
}

void Tooltip::updateLayout() {
    QFontMetrics metrics(font_);
    int w = metrics.width(text_) + 6; // 6 pixels more
    int h = metrics.height() + 6; // 6 pixels more

    resize(w,h);
    repaint();
}

void Tooltip::paintEvent(QPaintEvent* e) {
  QPainter painter(this);

  // Draws the bounding shadow.
  painter.setPen(backgroundColor_);
  painter.setFont(font_);

  for (int i = -2; i <= 2; ++i) {
    for (int j = -2; j <= 2; ++j) {
      painter.drawText(3 + i, 3 + fontSize_ + j, text_);
    }
  }

  // Now draws the normal text.
  painter.setPen(fontColor_);
  painter.setFont(font_);
  painter.drawText(3, 3 + fontSize_, text_);
}

}  // namespace ksmoothdock
