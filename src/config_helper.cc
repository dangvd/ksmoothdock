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

#include "config_helper.h"

#include <iostream>

#include <QFile>
#include <QStringList>

#include <KConfig>
#include <KWindowSystem>

namespace ksmoothdock {

constexpr char ConfigHelper::kSingleDockConfig[];
constexpr char ConfigHelper::kSingleDockOldConfig[];
constexpr char ConfigHelper::kSingleDockLaunchers[];

constexpr char ConfigHelper::kConfigPattern[];
constexpr char ConfigHelper::kGeneralCategory[];
constexpr char ConfigHelper::kAutoHide[];
constexpr char ConfigHelper::kPosition[];
constexpr char ConfigHelper::kScreen[];
constexpr char ConfigHelper::kShowApplicationMenu[];
constexpr char ConfigHelper::kShowClock[];
constexpr char ConfigHelper::kShowPager[];

constexpr char ConfigHelper::kAppearanceConfig[];
constexpr char ConfigHelper::kBackgroundColor[];
constexpr char ConfigHelper::kBorderColor[];
constexpr char ConfigHelper::kMaximumIconSize[];
constexpr char ConfigHelper::kMinimumIconSize[];
constexpr char ConfigHelper::kShowBorder[];
constexpr char ConfigHelper::kTooltipFontSize[];
constexpr char ConfigHelper::kApplicationMenuCategory[];
constexpr char ConfigHelper::kIcon[];
constexpr char ConfigHelper::kLabel[];
constexpr char ConfigHelper::kPagerCategory[];
constexpr char ConfigHelper::kWallpaper[];
constexpr char ConfigHelper::kClockCategory[];
constexpr char ConfigHelper::kUse24HourClock[];
constexpr char ConfigHelper::kFontScaleFactor[];

ConfigHelper::ConfigHelper(const QString& configDir)
    : configDir_{configDir} {
  if (!configDir_.exists()) {
    QDir::root().mkpath(configDir);
  }
}

std::vector<std::tuple<QString, QString>> ConfigHelper::findAllDockConfigs() {
  std::vector<std::tuple<QString, QString>> allConfigs;
  QStringList files = configDir_.entryList(
      {kConfigPattern}, QDir::Files, QDir::Name);
  if (files.isEmpty()) {
    return allConfigs;
  }

  for (int i = 0; i < files.size(); ++i) {
    const QString& configFile = files.at(i);
    allConfigs.push_back(std::make_tuple(
        dockConfigPath(configFile),
        getDockLaunchersPathForConfigFile(configFile)));
  }
  return allConfigs;
}

std::tuple<QString, QString> ConfigHelper::findNextDockConfigs() {
  for (int dockId = 1; ; ++dockId) {
    if (!configDir_.exists(dockConfigFile(dockId))) {
      return std::make_tuple(dockConfigPath(dockId),
                             dockLaunchersPath(dockId));
    }
  }
}

void ConfigHelper::convertConfig() {
  if (!configDir_.exists(kSingleDockConfig) ||
      configDir_.exists(kAppearanceConfig)) {
    return;
  }

  std::cout << "Converting single-dock config to multi-dock config"
            << std::endl;

  KConfig singleDockConfig(singleDockConfigPath(), KConfig::SimpleConfig);
  KConfig dock1Config(dockConfigPath(1), KConfig::SimpleConfig);

  KConfigGroup singleDockGeneral(&singleDockConfig, kGeneralCategory);
  KConfigGroup dock1General(&dock1Config, kGeneralCategory);
  copyEntry(kAutoHide, singleDockGeneral, &dock1General);
  copyEntry(kPosition, singleDockGeneral, &dock1General);
  copyEntry(kScreen, singleDockGeneral, &dock1General);
  copyEntry(kShowApplicationMenu, singleDockGeneral, &dock1General);
  copyEntry(kShowPager, singleDockGeneral, &dock1General);
  copyEntry(kShowClock, singleDockGeneral, &dock1General);

  dock1Config.sync();

  KConfig appearanceConfig(appearanceConfigPath(), KConfig::SimpleConfig);
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
    copyEntry(wallpaperConfigKey(desktop), singleDockPager,
              &appearancePager);
  }

  KConfigGroup singleDockClock(&singleDockConfig, kClockCategory);
  KConfigGroup appearanceClock(&appearanceConfig, kClockCategory);
  copyEntry(kUse24HourClock, singleDockClock, &appearanceClock);

  appearanceConfig.sync();

  configDir_.rename(kSingleDockConfig, kSingleDockOldConfig);
  configDir_.rename(kSingleDockLaunchers, dockLaunchersDir(1));
}

void ConfigHelper::copyLaunchersDir(const QString& launchersDir,
                                    const QString& newLaunchersDir) {
  QDir::root().mkpath(newLaunchersDir);
  QDir dir(launchersDir);
  QStringList files = dir.entryList({"*.desktop"}, QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    const auto srcFile = launchersDir + "/" + files.at(i);
    const auto destFile = newLaunchersDir + "/" + files.at(i);
    QFile::copy(srcFile, destFile);
  }
}

void ConfigHelper::removeLaunchersDir(const QString& launchersDir) {
  QDir dir(launchersDir);
  QStringList files = dir.entryList({"*.desktop"}, QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    const auto launcherFile = launchersDir + "/" + files.at(i);
    QFile::remove(launcherFile);
  }
  QDir::root().rmdir(launchersDir);
}

}  // namespace ksmoothdock
