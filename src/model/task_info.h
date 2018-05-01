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

#ifndef KSMOOTHDOCK_TASK_INFO_H_
#define KSMOOTHDOCK_TASK_INFO_H_

#include <QString>
#include <QPixmap>

namespace ksmoothdock {

struct TaskInfo {
  WId wId;
  QString name;
  QPixmap icon;
  int desktop;

  TaskInfo(WId wId2, const QString& name2, const QPixmap& icon2, int desktop2)
      : wId(wId2), name(name2), icon(icon2), desktop(desktop2) {}
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_TASK_INFO_H_
