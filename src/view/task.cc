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

#include "task.h"

#include <QPainterPath>
#include <QRect>

namespace ksmoothdock {

Task::Task(DockPanel *parent, MultiDockModel* model, const QString &label,
           Qt::Orientation orientation, const QPixmap &icon, int minSize,
           int maxSize, WId wId)
    : IconBasedDockItem(parent, label, orientation, icon, minSize, maxSize),
      model_(model),
      wId_(wId),
      demandAttention_(false),
      animationTimer_(std::make_unique<QTimer>(this)) {}

void Task::draw(QPainter *painter) const {
  painter->setRenderHint(QPainter::Antialiasing);
  QColor fillColor;
  QPainterPath path;
  if (active()) {
    fillColor = model_->backgroundColor().lighter();
    fillColor.setAlphaF(0.42);
    path.addRoundedRect(
        QRect(left_ - 5, top_ - 5, getWidth() + 10, getHeight() + 10),
        size_ / 8, size_ / 8);
  } else {
    fillColor = model_->borderColor();
    path.addRoundedRect(QRect(left_, top_ + getHeight() + 2, getWidth(), 3),
                        2, 2);
  }
  painter->fillPath(path, QBrush(fillColor));
  painter->setRenderHint(QPainter::Antialiasing, false);

  IconBasedDockItem::draw(painter);
}

void Task::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    if (active()) {
      KWindowSystem::minimizeWindow(wId_);
    } else {
      KWindowSystem::forceActiveWindow(wId_);
    }
  }
}

void Task::minimize() {}

void Task::maximize() {}

void Task::close() {}

}  // namespace ksmoothdock
