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

#include <iostream>

#include <KDesktopFile>
#include <KWindowSystem>

#include <utils/command_utils.h>

namespace ksmoothdock {

constexpr char MultiDockModel::kGeneralCategory[];
constexpr char MultiDockModel::kAutoHide[];
constexpr char MultiDockModel::kPosition[];
constexpr char MultiDockModel::kScreen[];
constexpr char MultiDockModel::kShowApplicationMenu[];
constexpr char MultiDockModel::kShowClock[];
constexpr char MultiDockModel::kShowPager[];
constexpr char MultiDockModel::kShowTaskManager[];
constexpr char MultiDockModel::kVisibility[];

constexpr char MultiDockModel::kBackgroundColor[];
constexpr char MultiDockModel::kBorderColor[];
constexpr char MultiDockModel::kMaximumIconSize[];
constexpr char MultiDockModel::kMinimumIconSize[];
constexpr char MultiDockModel::kSpacingFactor[];
constexpr char MultiDockModel::kShowBorder[];
constexpr char MultiDockModel::kTooltipFontSize[];
constexpr char MultiDockModel::kApplicationMenuCategory[];
constexpr char MultiDockModel::kIcon[];
constexpr char MultiDockModel::kLabel[];
constexpr char MultiDockModel::kStrut[];
constexpr char MultiDockModel::kPagerCategory[];
constexpr char MultiDockModel::kWallpaper[];
constexpr char MultiDockModel::kShowDesktopNumber[];
constexpr char MultiDockModel::kTaskManagerCategory[];
constexpr char MultiDockModel::kCurrentDesktopTasksOnly[];
constexpr char MultiDockModel::kCurrentScreenTasksOnly[];
constexpr char MultiDockModel::kClockCategory[];
constexpr char MultiDockModel::kUse24HourClock[];
constexpr char MultiDockModel::kFontScaleFactor[];

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
  if (convertConfig()) {
    appearanceConfig_.reparseConfiguration();
  }
  loadDocks();
  loadIconOverrideRules();
  connect(&applicationMenuConfig_, SIGNAL(configChanged()),
          this, SIGNAL(applicationMenuConfigChanged()));
}

void MultiDockModel::loadDocks() {
  // Dock ID starts from 1.
  int dockId = 1;
  dockConfigs_.clear();
  for (const auto& configs : configHelper_.findAllDockConfigs()) {
    const auto& configPath = std::get<0>(configs);
    const auto& launchersPath = std::get<1>(configs);
    dockConfigs_[dockId] = std::make_tuple(
        configPath,
        std::make_unique<KConfig>(configPath, KConfig::SimpleConfig),
        launchersPath,
        loadDockLaunchers(launchersPath));
    ++dockId;
  }
  nextDockId_ = dockId;
}

void MultiDockModel::addDock(PanelPosition position, int screen,
                             bool showApplicationMenu, bool showPager,
                             bool showTaskManager, bool showClock) {
  auto configs = configHelper_.findNextDockConfigs();
  auto dockId = addDock(configs, position, screen);
  setVisibility(dockId, kDefaultVisibility);
  setShowApplicationMenu(dockId, showApplicationMenu);
  setShowPager(dockId, showPager);
  setShowTaskManager(dockId, showTaskManager);
  setShowClock(dockId, showClock);
  emit dockAdded(dockId);

  if (dockCount() == 1) {
    setMinIconSize(kDefaultMinSize);
    setMaxIconSize(kDefaultMaxSize);
    QColor color(kDefaultBackgroundColor);
    color.setAlphaF(kDefaultBackgroundAlpha);
    setBackgroundColor(color);
    setShowBorder(kDefaultShowBorder);
    setBorderColor(QColor(kDefaultBorderColor));
    setTooltipFontSize(kDefaultTooltipFontSize);

    setApplicationMenuName(kDefaultApplicationMenuName);
    setApplicationMenuIcon(kDefaultApplicationMenuIcon);

    setUse24HourClock(kDefaultUse24HourClock);
    setClockFontScaleFactor(kDefaultClockFontScaleFactor);

    syncAppearanceConfig();
  }
  syncDockConfig(dockId);
  syncDockLaunchersConfig(dockId);
}

int MultiDockModel::addDock(const std::tuple<QString, QString>& configs,
                            PanelPosition position, int screen) {
  const auto dockId = nextDockId_;
  ++nextDockId_;
  const auto& configPath = std::get<0>(configs);
  const auto& launchersPath = std::get<1>(configs);
  dockConfigs_[dockId] = std::make_tuple(
      configPath,
      std::make_unique<KConfig>(configPath, KConfig::SimpleConfig),
      launchersPath,
      loadDockLaunchers(launchersPath));
  setPanelPosition(dockId, position);
  setScreen(dockId, screen);

  return dockId;
}

void MultiDockModel::cloneDock(int srcDockId, PanelPosition position,
                               int screen) {
  auto configs = configHelper_.findNextDockConfigs();

  // Clone the dock config and launchers.
  QFile::copy(dockConfigPath(srcDockId), std::get<0>(configs));
  ConfigHelper::copyLaunchersDir(dockLaunchersPath(srcDockId),
                                 std::get<1>(configs));

  auto dockId = addDock(configs, position, screen);
  emit dockAdded(dockId);

  syncDockConfig(dockId);
  syncDockLaunchersConfig(dockId);
}

void MultiDockModel::removeDock(int dockId) {
  QFile::remove(dockConfigPath(dockId));
  ConfigHelper::removeLaunchersDir(dockLaunchersPath(dockId));
  dockConfigs_.erase(dockId);
  // No need to emit a signal here.
}

bool MultiDockModel::hasPager() const {
  for (const auto& dock : dockConfigs_) {
    if (showPager(dock.first)) {
      return true;
    }
  }
  return false;
}

void MultiDockModel::syncDockLaunchersConfig(int dockId) {
  const auto& launchersPath = dockLaunchersPath(dockId);
  QDir launchersDir(launchersPath);
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    launchersDir.remove(files.at(i));
  }

  int launcherId = 1;
  for (const auto& item : dockLauncherConfigs(dockId)) {
    item.saveToFile(QString("%1/%2 - %3.desktop")
        .arg(launchersPath)
        .arg(launcherId, 2, 10, QChar('0'))
        .arg(item.name));
    ++launcherId;
  }
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

  return launchers;
}

std::vector<LauncherConfig> MultiDockModel::createDefaultLaunchers() {
  static constexpr int kNumItems = 7;
  static const char* kItems[kNumItems][3] = {
    // Name, icon, command.
    {"Show Desktop", "user-desktop", kShowDesktopCommand},
    {"Terminal", "utilities-terminal", "konsole"},
    {"File Manager", "system-file-manager", "dolphin"},
    {"Text Editor", "kate", "kate"},
    {"Web Browser", "internet-web-browser", "firefox"},
    {"Multimedia Player", "applications-multimedia", "dragon"},
    {"System Settings", "preferences-system", "systemsettings5"}
  };

  std::vector<LauncherConfig> launchers;
  launchers.reserve(kNumItems);
  for (int i = 0; i < kNumItems; ++i) {
    launchers.push_back(
        LauncherConfig(kItems[i][0], kItems[i][1], kItems[i][2]));
  }

  return launchers;
}

void MultiDockModel::loadIconOverrideRules() {
  iconOverrideRules_ =
      loadIconOverrideRulesFromFile<std::vector<IconOverrideRule>>(
          configHelper_.iconOverrideRulesPath());
}

void MultiDockModel::syncIconOverrideRules() {
  saveIconOverrideRulesToFile(configHelper_.iconOverrideRulesPath(),
                              iconOverrideRules_);
}

bool MultiDockModel::convertConfig() {
  if (!configHelper_.isSingleDockConfig()) {
    return false;
  }

  std::cout << "Converting single-dock config to multi-dock config"
            << std::endl;

  KConfig singleDockConfig(configHelper_.singleDockConfigPath(),
                           KConfig::SimpleConfig);
  KConfig dock1Config(configHelper_.dockConfigPathFromSingleDock(),
                      KConfig::SimpleConfig);

  KConfigGroup singleDockGeneral(&singleDockConfig, kGeneralCategory);
  KConfigGroup dock1General(&dock1Config, kGeneralCategory);
  copyEntry(kAutoHide, singleDockGeneral, &dock1General);
  copyEntry(kPosition, singleDockGeneral, &dock1General);
  copyEntry(kScreen, singleDockGeneral, &dock1General);
  copyEntry(kShowApplicationMenu, singleDockGeneral, &dock1General);
  copyEntry(kShowPager, singleDockGeneral, &dock1General);
  copyEntry(kShowClock, singleDockGeneral, &dock1General);

  dock1Config.sync();

  KConfig appearanceConfig(configHelper_.appearanceConfigPath(),
                           KConfig::SimpleConfig);
  KConfigGroup appearanceGeneral(&appearanceConfig, kGeneralCategory);
  copyEntry(kBackgroundColor, singleDockGeneral, &appearanceGeneral);
  copyEntry(kBorderColor, singleDockGeneral, &appearanceGeneral);
  copyEntry(kMaximumIconSize, singleDockGeneral, &appearanceGeneral);
  copyEntry(kMinimumIconSize, singleDockGeneral, &appearanceGeneral);
  copyEntry(kShowBorder, singleDockGeneral, &appearanceGeneral);
  copyEntry(kTooltipFontSize, singleDockGeneral, &appearanceGeneral);

  KConfigGroup singleDockAppMenu(&singleDockConfig, kApplicationMenuCategory);
  KConfigGroup appearanceAppMenu(&appearanceConfig, kApplicationMenuCategory);
  copyEntry(kIcon, singleDockAppMenu, &appearanceAppMenu);
  copyEntry(kLabel, singleDockAppMenu, &appearanceAppMenu);

  KConfigGroup singleDockPager(&singleDockConfig, kPagerCategory);
  KConfigGroup appearancePager(&appearanceConfig, kPagerCategory);
  const int numDesktops = KWindowSystem::numberOfDesktops();
  for (int desktop = 1; desktop <= numDesktops; ++desktop) {
    copyEntry(ConfigHelper::wallpaperConfigKey(desktop, 0 /* screen */),
              singleDockPager, &appearancePager);
  }

  KConfigGroup singleDockClock(&singleDockConfig, kClockCategory);
  KConfigGroup appearanceClock(&appearanceConfig, kClockCategory);
  copyEntry(kUse24HourClock, singleDockClock, &appearanceClock);

  appearanceConfig.sync();

  configHelper_.renameSingleDockConfigs();

  return true;
}

}  // namespace ksmoothdock
