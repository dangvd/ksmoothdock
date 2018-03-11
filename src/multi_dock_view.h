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

#ifndef KSMOOTHDOCK_MULTI_DOCK_VIEW_H_
#define KSMOOTHDOCK_MULTI_DOCK_VIEW_H_

#include <memory>
#include <unordered_map>

#include <QObject>

#include "dock_panel.h"
#include "multi_dock_model.h"

namespace ksmoothdock {

// The view.
class MultiDockView : public QObject {
  Q_OBJECT

 public:
  // No pointer ownership.
  MultiDockView(MultiDockModel* model);
  ~MultiDockView() = default;

  void show();

 public slots:
  void exit();

  void onDockAdded(int dockId);

 private:
  void loadData();

  // Creates a default dock if none exists.
  void createDefaultDock();

  MultiDockModel* model_;  // No ownership.
  std::unordered_map<int, std::unique_ptr<DockPanel>> docks_;
};

}  // namespace ksmoothdock

#endif // KSMOOTHDOCK_MULTI_DOCK_VIEW_H_
