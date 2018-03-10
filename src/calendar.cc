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

#include "calendar.h"

#include <QDate>

#include <KLocalizedString>

#include "dock_panel.h"

namespace ksmoothdock {

Calendar::Calendar(DockPanel* parent)
    : QDialog(parent),
      calendar_(this) {
  setWindowTitle(i18n("Calendar"));
  calendar_.setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
  resize(calendar_.sizeHint());
}

void Calendar::toggleCalendar() {
  calendar_.setSelectedDate(QDate::currentDate());
  setVisible(!isVisible());
}

}  // namespace ksmoothdock
