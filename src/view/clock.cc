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
#include <QIcon>
#include <QTime>
#include <QTimer>

#include <KLocalizedString>

#include "dock_panel.h"
#include "launcher.h"
#include <utils/font_utils.h>

namespace ksmoothdock {

constexpr float Clock::kWhRatio;
constexpr float Clock::kDelta;

Clock::Clock(DockPanel* parent, MultiDockModel* model,
             Qt::Orientation orientation, int minSize, int maxSize)
    : IconlessDockItem(parent, "" /* label */, orientation, minSize, maxSize,
                       kWhRatio),
      model_(model),
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

  painter->setFont(adjustFontSize(getWidth(), getHeight(), referenceTime,
                                  fontScaleFactor_));
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
  saveConfig();
}

void Clock::setFontScaleFactor(float fontScaleFactor) {
  fontScaleFactor_ = fontScaleFactor;
  largeFontAction_->setChecked(fontScaleFactor_ > kLargeClockFontScaleFactor - kDelta);
  mediumFontAction_->setChecked(
      fontScaleFactor_ > kMediumClockFontScaleFactor - kDelta &&
      fontScaleFactor_ < kMediumClockFontScaleFactor + kDelta);
  smallFontAction_->setChecked(fontScaleFactor_ < kSmallClockFontScaleFactor + kDelta);
}

void Clock::setLargeFont() {
  setFontScaleFactor(kLargeClockFontScaleFactor);
  saveConfig();
}

void Clock::setMediumFont() {
  setFontScaleFactor(kMediumClockFontScaleFactor);
  saveConfig();
}

void Clock::setSmallFont() {
  setFontScaleFactor(kSmallClockFontScaleFactor);
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

  QMenu* fontSize = menu_.addMenu(i18n("Font Size"));
  largeFontAction_ = fontSize->addAction(i18n("Large Font"),
                                                  this,
                                                  SLOT(setLargeFont()));
  largeFontAction_->setCheckable(true);
  mediumFontAction_ = fontSize->addAction(i18n("Medium Font"),
                                                  this,
                                                  SLOT(setMediumFont()));
  mediumFontAction_->setCheckable(true);
  smallFontAction_ = fontSize->addAction(i18n("Small Font"),
                                                  this,
                                                  SLOT(setSmallFont()));
  smallFontAction_->setCheckable(true);
}

void Clock::loadConfig() {
  set24HourClock(model_->use24HourClock());
  setFontScaleFactor(model_->clockFontScaleFactor());
}

void Clock::saveConfig() {
  model_->setUse24HourClock(use24HourClock_);
  model_->setClockFontScaleFactor(fontScaleFactor_);
  model_->saveAppearanceConfig();
}

}  // namespace ksmoothdock
