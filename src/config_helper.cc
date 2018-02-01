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
        getDockConfigPath(configFile),
        getDockLaunchersPathForConfigFile(configFile)));
  }
  return allConfigs;
}

std::tuple<QString, QString> ConfigHelper::findNextDockConfigs() {
  for (int dockIndex = 0; ; ++dockIndex) {
    if (!configDir_.exists(getDockConfigFile(dockIndex))) {
      return std::make_tuple(getDockConfigPath(dockIndex),
                             getDockLaunchersPath(dockIndex));
    }
  }
}

void ConfigHelper::convertConfig() {
  if (!configDir_.exists(kSingleDockConfig) ||
      configDir_.exists(kAppearanceConfig)) {
    return;
  }

  std::cout << "Converting single-dock config to multi-dock config";

  KConfig singleDockConfig(getSingleDockConfigPath(), KConfig::SimpleConfig);
  KConfig dock0Config(getDockConfigPath(0), KConfig::SimpleConfig);

  KConfigGroup singleDockGeneral(&singleDockConfig, kGeneralCategory);
  KConfigGroup dock0General(&dock0Config, kGeneralCategory);
  copyEntry(kAutoHide, singleDockGeneral, &dock0General);
  copyEntry(kPosition, singleDockGeneral, &dock0General);
  copyEntry(kScreen, singleDockGeneral, &dock0General);
  copyEntry(kShowApplicationMenu, singleDockGeneral, &dock0General);
  copyEntry(kShowPager, singleDockGeneral, &dock0General);
  copyEntry(kShowClock, singleDockGeneral, &dock0General);

  dock0Config.sync();

  KConfig appearanceConfig(getAppearanceConfigPath(), KConfig::SimpleConfig);
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
    copyEntry(getWallpaperConfigKey(desktop), singleDockPager,
              &appearancePager);
  }

  KConfigGroup singleDockClock(&singleDockConfig, kClockCategory);
  KConfigGroup appearanceClock(&appearanceConfig, kClockCategory);
  copyEntry(kUse24HourClock, singleDockClock, &appearanceClock);

  appearanceConfig.sync();

  configDir_.rename(kSingleDockConfig, kSingleDockOldConfig);
  configDir_.rename(kSingleDockLaunchers, getDockLaunchersDir(0));
}

}  // namespace ksmoothdock
