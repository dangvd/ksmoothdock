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
#include <regex>
#include <utility>

#include <QApplication>
#include <QDesktopWidget>

#include <KIconLoader>
#include <KWindowSystem>

namespace ksmoothdock {

TaskHelper::TaskHelper(const std::vector<IconOverrideRule>& iconOverrideRules)
    : iconOverrideRules_(iconOverrideRules) {
  currentDesktop_ = KWindowSystem::currentDesktop();
  connect(KWindowSystem::self(), &KWindowSystem::currentDesktopChanged,
          this, &TaskHelper::onCurrentDesktopChanged);
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

bool TaskHelper::isValidTask(WId wId, int screen, bool currentDesktopOnly) {
  if (!isValidTask(wId)) {
    return false;
  }

  if (screen >= 0 && getScreen(wId) != screen) {
    return false;
  }

  if (currentDesktopOnly) {
    KWindowInfo info(wId, NET::WMDesktop);
    if (info.desktop() != currentDesktop_ && !info.onAllDesktops()) {
      return false;
    }
  }

  return true;
}

TaskInfo TaskHelper::getTaskInfo(WId wId) {
  static constexpr int kIconLoadSize = 128;
  KWindowInfo info(wId, NET::WMVisibleName, NET::WM2WindowClass);

  const auto program = QString(info.windowClassName());
  const auto name = info.visibleName();
  QPixmap icon;
  QString iconName;

  for (const auto& rule : iconOverrideRules_) {
    if (rule.program.isEmpty() || program == rule.program) {
      // Check substring matching.
      if (name.contains(rule.window_name_regex)) {
        iconName = rule.icon;
        break;
      }

      // Check regex matching.
      std::regex re(rule.window_name_regex.toStdString());
      std::smatch m;
      std::string sname(name.toStdString());
      if (std::regex_match(sname, m, re)) {
        iconName = rule.icon;
        break;
      }
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
