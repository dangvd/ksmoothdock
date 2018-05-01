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

#ifndef KSMOOTHDOCK_TASK_UTILS_H_
#define KSMOOTHDOCK_TASK_UTILS_H_

#include <vector>

#include <QString>
#include <QPixmap>

namespace ksmoothdock {

struct TaskInfo {
  WId wId;
  QString name;
  QPixmap icon;

  TaskInfo(WId wId2, const QString& name2, const QPixmap& icon2)
      : wId(wId2), name(name2), icon(icon2) {}
};

// Loads running tasks.
std::vector<TaskInfo> loadTasks(int screen);

// Gets the screen that a task is running on.
int getScreen(WId wId);

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_TASK_UTILS_H_
