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

#include <algorithm>
#include <map>

#include <QApplication>
#include <QDesktopWidget>

#include <KIconLoader>
#include <KWindowSystem>

namespace ksmoothdock {

std::vector<TaskInfo> loadTasks(int screen) {
  std::vector<TaskInfo> tasks;
  for (const auto wId : KWindowSystem::windows()) {
    if (isValidTask(wId, screen)) {
      tasks.push_back(getTaskInfo(wId));
    }
  }

  std::stable_sort(tasks.begin(), tasks.end());
  return std::move(tasks);
}

bool isValidTask(WId wId) {
  if (!KWindowSystem::hasWId(wId)) {
    return false;
  }

  KWindowInfo info(wId, NET::WMState | NET::WMWindowType);
  const auto windowType = info.windowType(NET::DockMask | NET::DesktopMask);
  if (windowType == NET::Dock || windowType == NET::Desktop) {
    return false;
  }

  const auto state = info.state();
  if (state == NET::SkipTaskbar) {
    return false;
  }

  return true;
}

bool isValidTask(WId wId, int screen, bool currentDesktopOnly) {
  if (!isValidTask(wId)) {
    return false;
  }

  if (getScreen(wId) != screen) {
    return false;
  }

  KWindowInfo info(wId, NET::WMDesktop);
  if (currentDesktopOnly && !info.isOnCurrentDesktop()) {
    return false;
  }

  return true;
}

TaskInfo getTaskInfo(WId wId) {
  static constexpr int kIconLoadSize = 128;
  KWindowInfo info(wId, NET::WMVisibleName, NET::WM2WindowClass);

  const auto program = QString(info.windowClassName());
  const auto name = info.visibleName();
  QPixmap icon;
  QString iconName;

  static const std::map<QString, QString> icon_map = {
    {"Gmail -", "internet-mail"},
    {"Google Calendar -", "office-calendar"},
    {"Google Drive -", "nfs"},
    // {"YouTube -", "youtube"},
    // {"Facebook –", "facebook"},
  };
  for (const auto& entry : icon_map) {
    if (name.contains(entry.first)) {
      iconName = entry.second;
      break;
    }
  }
  if (!iconName.isEmpty()) {
    icon = KIconLoader::global()->loadIcon(iconName,
                                           KIconLoader::NoGroup, kIconLoadSize);
  } else {
    icon = KWindowSystem::icon(wId, kIconLoadSize, kIconLoadSize);
  }

  return TaskInfo(wId, program, name, icon);
}

int getScreen(WId wId) {
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
