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
#include <tuple>
#include <vector>

#include <QObject>
#include <QString>

#include "config_helper.h"
#include "ksmoothdock.h"

namespace ksmoothdock {

// Initializes and manages all the dock instances.
class DockManager : public QObject {
  Q_OBJECT

 public:
  DockManager(const QString& configDir);
  DockManager();

  ~DockManager();

  // Loads and initializes the dock instances.
  void init();

  // Shows all the dock instances.
  void show();

  // Adds a new dock in the specified position and screen.
  void addDock(PanelPosition position, int screen);

  // Clones an existing dock in the specified position and screen.
  void cloneDock(PanelPosition position, int screen, const QString& configFile,
                 const QString& launchersDir);

  // Removes a dock.
  void removeDock(KSmoothDock* dock,
                  const QString& configFile,
                  const QString& launchersDir);

  // Reloads all the dock instances.
  void reloadDocks();

  // Refreshes all the dock instances.
  void refreshDocks();

  // Returns true iff there's only one dock.
  bool hasOnlyOneDock() { return activeDocks_.size() == 1; }

 public slots:
  // Closes all dock instances and exits.
  void exit();

 private:
  // Loads all the dock instances.
  bool loadDocks();

  // Creates a default dock if none exists.
  void createDefaultDock();

  // Activates the newly created dock (last in the list).
  void activateNewDock();

  ConfigHelper configHelper_;

  // The list of all dock instances.
  std::vector<std::unique_ptr<KSmoothDock>> docks_;

  // The list of all active docks. An inactive dock is one that has been
  // requested to be removed.
  std::vector<KSmoothDock*> activeDocks_;

  // The list of configs of the docks to be removed.
  std::vector<std::tuple<QString, QString>> removedDocksConfigs_;

  friend class DockManagerTest;
  friend class ConfigDialogTest;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_MANAGER_H_
