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

#ifndef KSMOOTHDOCK_CALENDAR_H_
#define KSMOOTHDOCK_CALENDAR_H_

#include <QCalendarWidget>
#include <QDialog>

namespace ksmoothdock {

class KSmoothDock;

// A calendar widget. This is shown when the user clicks on the clock.
class Calendar : public QDialog {
 public:
  Calendar(KSmoothDock* parent);

  // Toggles showing the calendar.
  //
  // This also resets the selected date to the current date.
  void toggleCalendar();

 private:
  QCalendarWidget calendar_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_CALENDAR_H_
