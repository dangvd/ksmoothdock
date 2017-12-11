/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2017 Viet Dang (dangvd@gmail.com)
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

#ifndef KSMOOTHDOCK_DOCK_MANAGER_H_
#define KSMOOTHDOCK_DOCK_MANAGER_H_

#include <memory>
#include <vector>

#include <QString>

#include "ksmoothdock.h"

namespace ksmoothdock {

// Initializes and manages all the dock instances.
class DockManager {
 public:
  DockManager(const QString& configDir);
  DockManager();

  // Loads and initializes the dock instances.
  void init();

  // Shows all the dock instances.
  void show();

  // Adds a new dock in the specified position.
  void addDock(PanelPosition position);

 private:
  // Loads all the dock instances.
  bool loadDocks();

  // Creates a default dock if none exists.
  void createDefaultDock();

  // The config directory.
  const QString configDir_;

  // The list of all dock instances.
  std::vector<std::unique_ptr<KSmoothDock>> docks_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_MANAGER_H_
