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

#include "task_helper.h"

#include <algorithm>
#include <iostream>
#include <regex>
#include <utility>

#include <QApplication>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDesktopWidget>

#include <KWindowSystem>

namespace ksmoothdock {

bool TaskInfo::operator<(const TaskInfo& taskInfo) const {
  if (program == taskInfo.program) {
    // If same program, sort by creation time.
    bool found = false;
    for (const auto window : KWindowSystem::windows()) {
      if (window == wId) {
        found = true;
      } else if (window == taskInfo.wId) {
        return found;
      }
    }
    return true;
  }
  return program < taskInfo.program;
}

TaskHelper::TaskHelper(const std::vector<IconOverrideRule>& iconOverrideRules)
    : iconOverrideRules_(iconOverrideRules),
      currentDesktop_(KWindowSystem::currentDesktop()) {
  // Calling DBus to get current activity. This is more convenient than waiting for
  // KActivities::Consumer's status change then calling it.
  QDBusInterface activityManagerDBus("org.kde.ActivityManager", "/ActivityManager/Activities",
                                     "org.kde.ActivityManager.Activities");
  if (activityManagerDBus.isValid()) {
    const QDBusReply<QString> reply = activityManagerDBus.call("CurrentActivity");
    if (reply.isValid()) {
      currentActivity_ = reply.value();
    }
  }

  connect(KWindowSystem::self(), &KWindowSystem::currentDesktopChanged,
          this, &TaskHelper::onCurrentDesktopChanged);
  connect(&activityManager_, &KActivities::Consumer::currentActivityChanged,
          this, &TaskHelper::onCurrentActivityChanged);
}

std::vector<TaskInfo> TaskHelper::loadTasks(int screen, bool currentDesktopOnly) {
  std::vector<TaskInfo> tasks;
  for (const auto wId : KWindowSystem::windows()) {
    if (isValidTask(wId, screen, currentDesktopOnly)) {
      tasks.push_back(getTaskInfo(wId));
    }
  }

  std::stable_sort(tasks.begin(), tasks.end());
  return tasks;
}

bool TaskHelper::isValidTask(WId wId) {
  if (!KWindowSystem::hasWId(wId)) {
    return false;
  }

  KWindowInfo info(wId, NET::WMState | NET::WMWindowType);
  if (!info.valid()) {
    return false;
  }

  const auto windowType = info.windowType(NET::DockMask | NET::DesktopMask);
  if (windowType == NET::Dock || windowType == NET::Desktop) {
    return false;
  }

  const auto state = info.state();
  if (state & NET::SkipTaskbar) {
    return false;
  }

  return true;
}

bool TaskHelper::isValidTask(WId wId, int screen, bool currentDesktopOnly,
                             bool currentActivityOnly) {
  if (!isValidTask(wId)) {
    return false;
  }

  if (screen >= 0 && getScreen(wId) != screen) {
    return false;
  }

  if (currentDesktopOnly) {
    KWindowInfo info(wId, NET::WMDesktop);
    if (!info.valid() || (info.desktop() != currentDesktop_ && !info.onAllDesktops())) {
      return false;
    }
  }

  if (currentActivityOnly) {
    KWindowInfo info(wId, 0, NET::WM2Activities);
    if (!info.valid() ||
        (!info.activities().empty() && !info.activities().contains(currentActivity_))) {
      return false;
    }
  }
  
  return true;
}

/* static */ TaskInfo TaskHelper::getBasicTaskInfo(WId wId) {
  KWindowInfo info(wId, 0, NET::WM2WindowClass);
  const auto program = QString(info.windowClassName());
  return TaskInfo(wId, program);
}

TaskInfo TaskHelper::getTaskInfo(WId wId) const {
  static constexpr int kIconLoadSize = 128;
  KWindowInfo info(wId, NET::WMVisibleName | NET::WMState, NET::WM2WindowClass);

  const auto program = QString(info.windowClassName());
  const auto name = info.visibleName();
  QPixmap icon = KWindowSystem::icon(wId, kIconLoadSize, kIconLoadSize, true /* scale */);

  return TaskInfo(wId, program, name, icon, info.state() == NET::DemandsAttention);
}

int TaskHelper::getScreen(WId wId) {
  const auto* desktop = QApplication::desktop();
  const auto screenCount = desktop->screenCount();
  if (screenCount == 1) {
    return 0;
  }

  KWindowInfo info(wId, NET::WMFrameExtents);
  const auto& geometry = info.frameGeometry();
  for (int screen = 0; screen < screenCount; ++screen) {
    const auto& screenGeometry = desktop->screenGeometry(screen);
    if (screenGeometry.intersects(geometry)) {
      return screen;
    }
  }
  return -1;
}

}  // namespace ksmoothdock
