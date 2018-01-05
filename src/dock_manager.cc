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

#include "dock_manager.h"

#include <iostream>

#include <QDir>
#include <QStringList>

namespace ksmoothdock {

DockManager::DockManager(const QString &configDir)
    : configDir_(configDir) {}

DockManager::DockManager() : DockManager(QDir::homePath() + "/.ksmoothdock") {}

void DockManager::init() {
  if (!loadDocks()) {
    createDefaultDock();
  }

  for (const auto& dock : docks_) {
    dock->init();
  }
}

void DockManager::show() {
  for (const auto& dock : docks_) {
    dock->show();
  }
}

void DockManager::addDock(PanelPosition position) {
  QDir configDir(configDir_);
  for (int dockIndex = 0; ; ++dockIndex) {
    // Find an unused dock index and use it for the new dock.
    QString configFile = (dockIndex == 0)
        ? "ksmoothdockrc"
        : "ksmoothdockrc" + QString::number(dockIndex);
    if (!configDir.exists(configFile)) {
      QString launcherDir = configFile;
      launcherDir.replace("ksmoothdockrc", "launchers");
      docks_.push_back(std::unique_ptr<KSmoothDock>(new KSmoothDock(
          this,
          configDir_ + "/" + configFile,
          configDir_ + "/" + launcherDir,
          position)));
      docks_.back()->init();
      docks_.back()->show();
      return;
    }
  }
}

bool DockManager::loadDocks() {
  docks_.clear();
  if (!QDir::root().exists(configDir_)) {
    std::cerr << "Config directory does not exist" << std::endl;
    return false;
  }

  QDir configDir(configDir_);
  QStringList files = configDir.entryList({"ksmoothdockrc*"}, QDir::Files,
                                          QDir::Name);
  if (files.isEmpty()) {
    std::cerr << "Couldn't find any ksmoothdockrc* file in the config directory"
              << std::endl;
    return false;
  }

  for (int i = 0; i < files.size(); ++i) {
    const QString& configFile = files.at(i);
    QString launcherDir = configFile;
    launcherDir.replace("ksmoothdockrc", "launchers");
    docks_.push_back(std::unique_ptr<KSmoothDock>(
                         new KSmoothDock(this,
                                         configDir_ + "/" + configFile,
                                         configDir_ + "/" + launcherDir)));
  }
  return true;
}

void DockManager::createDefaultDock() {
  docks_.push_back(std::unique_ptr<KSmoothDock>(new KSmoothDock(this)));
}

}  // namespace ksmoothdock
