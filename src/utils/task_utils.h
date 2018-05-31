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

#include "model/icon_override_rule.h"

namespace ksmoothdock {

struct TaskInfo {
  WId wId;
  QString program;  // e.g. dolphin
  QString name;  // e.g. home -- Dolphin
  QPixmap icon;

  TaskInfo(WId wId2, const QString& program2, const QString& name2,
           const QPixmap& icon2)
      : wId(wId2), program(program2), name(name2), icon(icon2) {}
  TaskInfo(const TaskInfo& taskInfo) = default;
  TaskInfo& operator=(const TaskInfo& taskInfo) = default;

  bool operator<(const TaskInfo& taskInfo) const {
    return program < taskInfo.program;
  }
};

// Loads running tasks.
std::vector<TaskInfo>
loadTasks(int screen, const std::vector<IconOverrideRule>& icon_override_rules);

// Whether the task is valid for showing on the task manager.
bool isValidTask(WId wId);

// Whether the task is valid for showing on the task manager on specific screen.
bool isValidTask(WId wId, int screen, bool currentDesktopOnly = true);

TaskInfo getTaskInfo(WId wId,
                     const std::vector<IconOverrideRule>& icon_override_rules);

// Gets the screen that a task is running on.
int getScreen(WId wId);

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_TASK_UTILS_H_
