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

#include <algorithm>

#include <QColor>
#include <QDate>
#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QRect>
#include <QTime>
#include <QTimer>

#include <KConfigGroup>
#include <KLocalizedString>

#include "config_helper.h"
#include "ksmoothdock.h"
#include "launcher.h"

namespace ksmoothdock {

Clock::Clock(KSmoothDock* parent, Qt::Orientation orientation, int minSize,
             int maxSize, KConfig* config)
    : IconlessDockItem(parent, "" /* label */, orientation, minSize, maxSize,
                       kWhRatio),
      config_(config),
      calendar_(parent) {
  createMenu();
  loadConfig();

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
  timer->start(1000);  // update the time every second.
}

void Clock::draw(QPainter *painter) const {
  const QString time = QTime::currentTime().toString(timeFormat_);
  // The reference time used to calculate the font size.
  const QString referenceTime = QTime(8, 8).toString(timeFormat_);

  QFont font;
  QFontMetrics metrics(font);
  const QRect& rect = metrics.tightBoundingRect(referenceTime);
  // Scale the font size according to the size of the dock.
  font.setPointSize(std::min(
      font.pointSize() * getWidth() / rect.width(),
      font.pointSize() * getHeight() / rect.height()));
  painter->setFont(font);
  painter->setRenderHint(QPainter::TextAntialiasing);

  if (size_ > minSize_) {
    painter->setPen(Qt::black);
    for (int i = -2; i <= 2; ++i) {
      for (int j = -2; j <= 2; ++j) {
        painter->drawText(left_ + i, top_ + j, getWidth(), getHeight(),
                          Qt::AlignCenter, time);
      }
    }
  }

  painter->setPen(Qt::white);
  painter->drawText(left_, top_, getWidth(), getHeight(), Qt::AlignCenter,
                    time);
}

void Clock::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    calendar_.toggleCalendar();
  } else if (e->button() == Qt::RightButton) {
    menu_.popup(e->globalPos());
  }
}

QString Clock::getLabel() const {
  return QDate::currentDate().toString(Qt::SystemLocaleLongDate);
}

void Clock::updateTime() {
  parent_->update();
}

void Clock::setDateAndTime() {
  Launcher::launch("kcmshell5 clock");
}

void Clock::set24HourClock(bool enabled) {
  use24HourClock_ = enabled;
  timeFormat_ = enabled ? "hh:mm" : "hh:mm AP";
  use24HourClockAction_->setChecked(enabled);
}

void Clock::toggle24HourClock() {
  set24HourClock(!use24HourClock_);
  parent_->update();
  saveConfig();
}

void Clock::createMenu() {
  menu_.addAction(QIcon::fromTheme("preferences-system-time"),
                  i18n("Date and Time &Settings"),
                  this,
                  SLOT(setDateAndTime()));
  use24HourClockAction_ = menu_.addAction(i18n("Use 24-hour Clock"),
                                          this,
                                          SLOT(toggle24HourClock()));
  use24HourClockAction_->setCheckable(true);
}

void Clock::loadConfig() {
  KConfigGroup group(config_, ConfigHelper::kClockCategory);
  set24HourClock(group.readEntry(ConfigHelper::kUse24HourClock, true));
}

void Clock::saveConfig() {
  KConfigGroup group(config_, ConfigHelper::kClockCategory);
  group.writeEntry(ConfigHelper::kUse24HourClock, use24HourClock_);
  config_->sync();
}

}  // namespace ksmoothdock
