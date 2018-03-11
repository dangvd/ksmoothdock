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

#include "multi_dock_model.h"

#include <KDesktopFile>

#include "command_utils.h"

namespace ksmoothdock {

LauncherConfig::LauncherConfig(const QString& desktopFile) {
  KDesktopFile file(desktopFile);
  name = file.readName();
  icon = file.readIcon();
  command = filterFieldCodes(file.entryMap("Desktop Entry")["Exec"]);
}

void LauncherConfig::saveToFile(const QString &filePath) const {
  KConfig config(filePath, KConfig::SimpleConfig);
  KConfigGroup group(&config, "Desktop Entry");
  group.writeEntry("Name", name);
  group.writeEntry("Icon", icon);
  group.writeEntry("Exec", command);
  group.writeEntry("Type", "Application");
  group.writeEntry("Terminal", false);
  config.sync();
}

MultiDockModel::MultiDockModel(const QString& configDir)
    : configHelper_(configDir),
      appearanceConfig_(configHelper_.appearanceConfigPath(),
                        KConfig::SimpleConfig) {
  if (configHelper_.convertConfig()) {
    appearanceConfig_.reparseConfiguration();
  }

  // Dock ID starts from 1.
  int dockId = 1;
  for (const auto& configs : configHelper_.findAllDockConfigs()) {
    dockConfigs_[dockId] = std::make_tuple(
        std::make_unique<KConfig>(std::get<0>(configs), KConfig::SimpleConfig),
        loadDockLaunchers(std::get<1>(configs)));
    ++dockId;
  }
}

MultiDockModel::~MultiDockModel() {
  appearanceConfig_.sync();
}

void MultiDockModel::addDock(PanelPosition position, int screen) {
  auto configs = configHelper_.findNextDockConfigs();
  addDock(configs, position, screen);
}

void MultiDockModel::addDock(const std::tuple<QString, QString>& configs,
                             PanelPosition position, int screen) {
  // Dock ID starts from 1.
  const auto dockId = dockConfigs_.size() + 1;
  dockConfigs_[dockId] = std::make_tuple(
      std::make_unique<KConfig>(std::get<0>(configs), KConfig::SimpleConfig),
      loadDockLaunchers(std::get<1>(configs)));
  setPanelPosition(dockId, position);
  setScreen(dockId, screen);
  emit dockAdded(dockId);
}

void MultiDockModel::cloneDock(int srcDockId, PanelPosition position,
                               int screen) {
  auto configs = configHelper_.findNextDockConfigs();

  // Clone the dock config and launchers.
  QFile::copy(configHelper_.dockConfigPath(srcDockId), std::get<0>(configs));
  ConfigHelper::copyLaunchersDir(configHelper_.dockLaunchersPath(srcDockId),
                                 std::get<1>(configs));

  addDock(configs, position, screen);
}

void MultiDockModel::removeDock(int dockId) {
  dockConfigs_.erase(dockId);
  emit dockRemoved(dockId);
}

void MultiDockModel::saveLauncherConfigs(int dockId) {
  const auto& launchersPath = configHelper_.dockLaunchersPath(dockId);
  QDir launchersDir(launchersPath);
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    launchersDir.remove(files.at(i));
  }

  int launcherId = 1;
  for (const auto& item : launcherConfigs(dockId)) {
    item.saveToFile(QString("%1/%2 - %3.desktop")
        .arg(launchersPath)
        .arg(launcherId, 2, 10, QChar('0'))
        .arg(item.name));
    ++launcherId;
  }

  emit dockLaunchersChanged(dockId);
}

std::vector<LauncherConfig> MultiDockModel::loadDockLaunchers(
    const QString& dockLaunchersPath) {
  QDir launchersDir(dockLaunchersPath);
  QStringList files = launchersDir.entryList({"*.desktop"}, QDir::Files,
                                             QDir::Name);
  if (files.empty()) {
    return createDefaultLaunchers();
  }

  std::vector<LauncherConfig> launchers;
  launchers.reserve(files.size());
  for (int i = 0; i < files.size(); ++i) {
    const QString& desktopFile = dockLaunchersPath + "/" + files.at(i);
    launchers.push_back(LauncherConfig(desktopFile));
  }

  return std::move(launchers);
}

std::vector<LauncherConfig> MultiDockModel::createDefaultLaunchers() {
  static constexpr int kNumItems = 7;
  static const char* kItems[kNumItems][3] = {
    // Name, icon, command.
    {"Show Desktop", "user-desktop", kShowDesktopCommand},
    {"Terminal", "utilities-terminal", "konsole"},
    {"File Manager", "system-file-manager", "dolphin"},
    {"Text Editor", "kate", "kate"},
    {"Web Browser", "applications-internet", "firefox"},
    {"Audio Player", "audio-headphones", "amarok"},
    {"System Settings", "preferences-system", "systemsettings5"}
  };

  std::vector<LauncherConfig> launchers;
  launchers.reserve(kNumItems);
  for (int i = 0; i < kNumItems; ++i) {
    launchers.push_back(
        LauncherConfig(kItems[i][0], kItems[i][1], kItems[i][2]));
  }

  return std::move(launchers);
}

}  // namespace ksmoothdock
