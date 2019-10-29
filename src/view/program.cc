/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2019 Viet Dang (dangvd@gmail.com)
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

#include "program.h"

#include <iostream>

#include <QGuiApplication>
#include <QProcess>
#include <QTimer>

#include <KDesktopFile>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowSystem>

#include "dock_panel.h"
#include <utils/draw_utils.h>

namespace ksmoothdock {

Program::Program(DockPanel* parent, MultiDockModel* model, const QString& label,
    Qt::Orientation orientation, const QString& iconName, int minSize,
    int maxSize, const QString& command)
    : IconBasedDockItem(parent, label, orientation, iconName, minSize, maxSize),
      model_(model),
      command_(command),
      launching_(false),
      pinned_(true) {
  name_ = command_.left(command_.indexOf(" "));
  createMenu();
}

Program::Program(DockPanel* parent, MultiDockModel* model, const QString& label,
    Qt::Orientation orientation, const QPixmap& icon, int minSize,
    int maxSize, const QString& command)
    : IconBasedDockItem(parent, label, orientation, icon, minSize, maxSize),
      model_(model),
      command_(command),
      launching_(false),
      pinned_(false) {
  name_ = command_.left(command_.indexOf(" "));
  createMenu();
}

void Program::draw(QPainter *painter) const {
  if (launching_ || (!tasks_.empty() && active())) {
    drawHighlightedIcon(model_->backgroundColor(), left_, top_, getWidth(), getHeight(),
                        5, size_ / 8, painter);
  } else if (!tasks_.empty()) {
    drawHighlightedIcon(model_->backgroundColor(), left_, top_, getWidth(), getHeight(),
                        5, size_ / 8, painter, 0.25);
  }
  IconBasedDockItem::draw(painter);
}

void Program::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) { // Run the application.
    if (command_ == kShowDesktopCommand) {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    } else if (isCommandLockScreen(command_)) {
      parent_->leaveEvent(nullptr);
      QTimer::singleShot(500, []() {
        lockScreen();
      });
    } else {
      if (tasks_.empty()) {
        launch();
      } else {
        const auto mod = QGuiApplication::keyboardModifiers();
        if (mod & Qt::ShiftModifier) {
          launch();
        } else {
          const auto activeTask = getActiveTask();
          if (activeTask >= 0) {
            if (tasks_.size() == 1) {
              KWindowSystem::minimizeWindow(tasks_[0].wId);
            } else {
              // Cycles through tasks.
              auto nextTask = (activeTask < static_cast<int>(tasks_.size() - 1)) ?
                    (activeTask + 1) : 0;
              KWindowSystem::forceActiveWindow(tasks_[nextTask].wId);
            }
          } else {
            KWindowSystem::forceActiveWindow(tasks_[0].wId);
          }
        }
      }
    }
  } else if (e->button() == Qt::RightButton) {
    menu_.popup(e->globalPos());
  }
}

bool Program::addTask(const TaskInfo& task) {
  if (name_ == task.program) {
    tasks_.push_back(ProgramTask(task.wId, task.name, task.demandsAttention));
    return true;
  }
  return false;
}

bool Program::removeTask(WId wId) {
  for (int i = 0; i < static_cast<int>(tasks_.size()); ++i) {
    if (tasks_[i].wId == wId) {
      tasks_.erase(tasks_.begin() + i);
      return true;
    }
  }
  return false;
}

bool Program::hasTask(WId wId) {
  for (const auto& task : tasks_) {
    if (task.wId == wId) {
      return true;
    }
  }
  return false;
}

bool Program::beforeTask(const TaskInfo& task) {
  return name_ < task.program;
}

void Program::launch() {
  launch(command_);
  parent_->showWaitCursor();
  setLaunching(true);
  parent_->update();
  QTimer::singleShot(500, [this]() {
    setLaunching(false);
    parent_->update();
  });
}

void Program::pinUnpin() {
  pinned_ = !pinned_;
  if (pinned_) {
    // TODO
    model_->addLauncher(parent_->dockId(), LauncherConfig(name_, "xapp", command_));
  } else {  // !pinned
    model_->removeLauncher(parent_->dockId(), command_);
    if (shouldBeRemoved()) {
      // TODO
    }
  }
}

void Program::launch(const QString& command) {
  if (!QProcess::startDetached(command)) {
    KMessageBox::error(nullptr,
        i18n("Could not run command: ") + command);
  }
}

void Program::createMenu() {
  menu_.addAction(QIcon::fromTheme("configure"), i18n("Edit &Launchers"), parent_,
                  [this] { parent_->showEditLaunchersDialog(); });

  pinAction_ = menu_.addAction(
      i18n("Pinned"), this,
      [this] {
        pinUnpin();
      });
  pinAction_->setCheckable(true);
  pinAction_->setChecked(pinned_);

  menu_.addSeparator();
  parent_->addPanelSettings(&menu_);
}

}  // namespace ksmoothdock
