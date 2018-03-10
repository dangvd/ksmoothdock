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

#include "multi_dock_view.h"

#include "welcome_dialog.h"

namespace ksmoothdock {

MultiDockView::MultiDockView(MultiDockModel* model)
    : model_(model) {
  connect(model_, SIGNAL(dockAdded(int)), this, SLOT(onDockAdded(int)));
  connect(model_, SIGNAL(dockRemoved(int)), this, SLOT(onDockRemoved(int)));
  loadData();
}

void MultiDockView::show() {
  for (const auto& dock : docks_) {
    dock.second->show();
  }
}

void MultiDockView::exit() {
  for (const auto& dock : docks_) {
    dock.second->close();
  }
}

void MultiDockView::onDockAdded(int dockId) {
  docks_[dockId] = std::make_unique<DockPanel>(this, model_, dockId);
  docks_[dockId]->show();
}

void MultiDockView::onDockRemoved(int dockId) {
  docks_[dockId]->close();
}

void MultiDockView::loadData() {
  docks_.clear();
  for (int id = 0; id < model_->dockCount(); ++id) {
    docks_[id] = std::make_unique<DockPanel>(this, model_, id);
  }

  if (docks_.empty()) {
    WelcomeDialog welcome(model_);
    welcome.exec();
  }
}

}  // namespace ksmoothdock
