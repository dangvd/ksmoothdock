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

#include "task_utils.h"

#include <QApplication>
#include <QDesktopWidget>

#include <KWindowSystem>

namespace ksmoothdock {

std::vector<TaskInfo> loadTasks(int screen) {
  std::vector<TaskInfo> tasks;
  for (const auto wId : KWindowSystem::windows()) {
    if (isValidTask(wId, screen)) {
      tasks.push_back(getTaskInfo(wId));
    }
  }

  return std::move(tasks);
}

bool isValidTask(WId wId) {
  if (!KWindowSystem::hasWId(wId)) {
    return false;
  }

  KWindowInfo info(wId, NET::WMDesktop | NET::WMState | NET::WMWindowType);
  const auto windowType = info.windowType(NET::DockMask | NET::DesktopMask);
  if (windowType == NET::Dock || windowType == NET::Desktop) {
    return false;
  }

  const auto state = info.state();
  if (state == NET::SkipTaskbar) {
    return false;
  }

  if (!info.isOnCurrentDesktop()) {
    return false;
  }

  return true;
}

bool isValidTask(WId wId, int screen) {
  if (!isValidTask(wId)) {
    return false;
  }

  if (getScreen(wId) != screen) {
    return false;
  }

  return true;
}

TaskInfo getTaskInfo(WId wId) {
  static constexpr int kIconLoadSize = 128;
  KWindowInfo info(wId, NET::WMVisibleName);
  return TaskInfo(wId, info.visibleName(),
                  KWindowSystem::icon(wId, kIconLoadSize, kIconLoadSize));
}

int getScreen(WId wId) {
  KWindowInfo info(wId, NET::WMFrameExtents);
  const auto& geometry = info.frameGeometry();
  const auto* desktop = QApplication::desktop();
  for (int screen = 0; screen < desktop->screenCount(); ++screen) {
    const auto& screenGeometry = desktop->screenGeometry(screen);
    if (screenGeometry.contains(geometry.topLeft())) {
      return screen;
    }
  }
  return -1;
}

}  // namespace ksmoothdock
