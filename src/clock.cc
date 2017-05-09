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

#include "clock.h"

#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QTime>
#include <QTimer>

#include <KConfigGroup>

#include "ksmoothdock.h"

namespace ksmoothdock {

Clock::Clock(KSmoothDock* parent, Qt::Orientation orientation, int minSize,
             int maxSize, KConfig* config)
    : IconlessDockItem(parent, "" /* label */, orientation, minSize, maxSize,
                       0.0 /* whRatio */),
      config_(config) {
  KConfigGroup group(config_, "Clock");
  use24HourClock_ = group.readEntry("use24HourClock", true);
  whRatio_ = use24HourClock_ ? kWhRatio24HourClock : kWhRatio12HourClock;

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
  timer->start(1000);  // update the time every second.
}

void Clock::draw(QPainter *painter) const {
  const QString time = QTime::currentTime().toString("hh:mm");

  QFont font;
  QFontMetrics metrics(font);
  // Scale the font size according to the size of the dock.
  font.setPointSize(font.pointSize() * getHeight()
                    / metrics.tightBoundingRect(time).height());
  font.setPointSize(static_cast<int>(font.pointSize() * 0.8));
  painter->setFont(font);

  painter->setPen(QColor("white"));
  painter->drawText(left_, top_, getWidth(), getHeight(), Qt::AlignCenter,
                    time);
}

void Clock::mousePressEvent(QMouseEvent *e) {}

void Clock::updateTime() {
  parent_->update();
}

}  // namespace ksmoothdock
