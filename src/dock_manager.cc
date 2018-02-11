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
#include <memory>

#include <QDir>
#include <QFile>
#include <QStringList>

#include <KConfig>

#include "welcome_dialog.h"

namespace ksmoothdock {

DockManager::DockManager(const QString &configDir)
    : configHelper_(configDir) {}

DockManager::DockManager() : DockManager(QDir::homePath() + "/.ksmoothdock") {}

void DockManager::init() {
  configHelper_.convertConfig();

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
  auto configs = configHelper_.findNextDockConfigs();
  docks_.push_back(std::make_unique<KSmoothDock>(
      this,
      std::get<0>(configs),
      std::get<1>(configs),
      configHelper_.getAppearanceConfigPath(),
      position));
  docks_.back()->init();
  docks_.back()->show();
}

void DockManager::cloneDock(PanelPosition position, const QString& configFile,
                            const QString& launchersDir) {
  auto configs = configHelper_.findNextDockConfigs();
  const auto& newConfigFile = std::get<0>(configs);
  const auto& newLaunchersDir = std::get<1>(configs);
  QFile::copy(configFile, newConfigFile);

  // Clone the dock config and launchers.
  QDir::root().mkpath(newLaunchersDir);
  QDir dir(launchersDir);
  QStringList files = dir.entryList({"*.desktop"}, QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    const auto srcFile = launchersDir + "/" + files.at(i);
    const auto destFile = newLaunchersDir + "/" + files.at(i);
    QFile::copy(srcFile, destFile);
  }

  docks_.push_back(std::make_unique<KSmoothDock>(
      this,
      newConfigFile,
      newLaunchersDir,
      configHelper_.getAppearanceConfigPath(),
      position));
  docks_.back()->init();
  docks_.back()->show();
}

void DockManager::reloadDocks() {
  for (const auto& dock : docks_) {
    dock->reload();
  }
}

void DockManager::exit() {
  for (const auto& dock : docks_) {
    dock->close();
  }
}

bool DockManager::loadDocks() {
  docks_.clear();
  for (const auto& configs : configHelper_.findAllDockConfigs()) {
    docks_.push_back(std::make_unique<KSmoothDock>(
        this,
        std::get<0>(configs),
        std::get<1>(configs),
        configHelper_.getAppearanceConfigPath()));
  }
  return !docks_.empty();
}

void DockManager::createDefaultDock() {
  WelcomeDialog welcome;
  welcome.exec();
  PanelPosition position =
      static_cast<PanelPosition>(welcome.getSelectedPositionIndex());
  docks_.push_back(std::make_unique<KSmoothDock>(this,
                   configHelper_.getDockConfigPath(0),
                   configHelper_.getDockLaunchersPath(0),
                   configHelper_.getAppearanceConfigPath(),
                   position));
}

}  // namespace ksmoothdock
