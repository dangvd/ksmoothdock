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

#ifndef KSMOOTHDOCK_TASK_HELPER_H_
#define KSMOOTHDOCK_TASK_HELPER_H_

#include <vector>

#include <QObject>
#include <QPixmap>
#include <QString>

#include <kactivities/consumer.h>

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

class TaskHelper : public QObject {
  Q_OBJECT

 public:
  TaskHelper(const std::vector<IconOverrideRule>& iconOverrideRules);

  // Loads running tasks.
  //
  // Args:
  //   screen: screen index to load, or -1 if loading for all screens.
  std::vector<TaskInfo> loadTasks(int screen, bool currentDesktopOnly);

  // Whether the task is valid for showing on the task manager.
  bool isValidTask(WId wId);

  // Whether the task is valid for showing on the task manager on specific screen.
  bool isValidTask(WId wId, int screen, bool currentDesktopOnly = true,
                   bool currentActivityOnly = true);

  TaskInfo getTaskInfo(WId wId);

  // Gets the screen that a task is running on.
  int getScreen(WId wId);

 public slots:
  void onCurrentDesktopChanged(int desktop) {
    currentDesktop_ = desktop;
  }

  void onCurrentActivityChanged(QString activity) {
    currentActivity_ = activity;
  }

 private:
  const std::vector<IconOverrideRule>& iconOverrideRules_;

  // KWindowSystem::currentDesktop() is buggy sometimes, for example,
  // on windowAdded() event, so we store it here ourselves.
  int currentDesktop_;

  // ID of the current activity.
  QString currentActivity_;

  KActivities::Consumer activityManager_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_TASK_HELPER_H_
