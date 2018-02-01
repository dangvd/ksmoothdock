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

#ifndef KSMOOTHDOCK_CONFIG_HELPER_H_
#define KSMOOTHDOCK_CONFIG_HELPER_H_

#include <tuple>
#include <vector>

#include <QDir>

#include <KConfigGroup>

namespace ksmoothdock {

// Helper class for working with configurations.
class ConfigHelper  {
 public:
  // [Deprecated] single-dock configs.
  // ksmoothdockrc will be renamed to ksmoothdockrc.old when it is converted
  // to the new multi-dock configs.
  static constexpr char kSingleDockConfig[] = "ksmoothdockrc";
  static constexpr char kSingleDockOldConfig[] = "ksmoothdockrc.old";
  static constexpr char kSingleDockLaunchers[] = "launchers";

  // Individual dock configs.

  static constexpr char kConfigPattern[] = "panel_*.conf";

  static constexpr char kGeneralCategory[] = "General";
  static constexpr char kAutoHide[] = "autoHide";
  static constexpr char kPosition[] = "position";
  static constexpr char kScreen[] = "screen";
  static constexpr char kShowApplicationMenu[] = "showApplicationMenu";
  static constexpr char kShowClock[] = "showClock";
  static constexpr char kShowPager[] = "showPager";

  // Global appearance configs.

  static constexpr char kAppearanceConfig[] = "appearance.conf";

  // General category.
  static constexpr char kBackgroundColor[] = "backgroundColor";
  static constexpr char kBorderColor[] = "borderColor";
  static constexpr char kMaximumIconSize[] = "maximumIconSize";
  static constexpr char kMinimumIconSize[] = "minimumIconSize";
  static constexpr char kShowBorder[] = "showBorder";
  static constexpr char kTooltipFontSize[] = "tooltipFontSize";

  static constexpr char kApplicationMenuCategory[] = "Application Menu";
  static constexpr char kIcon[] = "icon";
  static constexpr char kLabel[] = "label";

  static constexpr char kPagerCategory[] = "Pager";
  static constexpr char kWallpaper[] = "wallpaper";

  static constexpr char kClockCategory[] = "Clock";
  static constexpr char kUse24HourClock[] = "use24HourClock";

  explicit ConfigHelper(const QString& configDir);
  ~ConfigHelper() = default;

  // Gets the config file path of the old single-dock config.
  QString getSingleDockConfigPath() const {
    return configDir_.filePath(kSingleDockConfig);
  }

  // Gets the launchers dir path of the old single-dock config.
  QString getSingleDockLaunchersPath() const {
    return configDir_.filePath(kSingleDockLaunchers);
  }

  // Gets the config file name of a dock.
  QString getDockConfigFile(int dockIndex) const {
    return QString("panel_") + QString::number(dockIndex) + ".conf";
  }

  // Gets the config file path of a dock.
  QString getDockConfigPath(int dockIndex) const {
    return configDir_.filePath(getDockConfigFile(dockIndex));
  }

  QString getDockConfigPath(QString configFile) const {
    return configDir_.filePath(configFile);
  }

  static QString getDockLaunchersDir(int dockIndex) {
    return QString("panel_") + QString::number(dockIndex) + "_launchers";
  }

  // Gets the launchers dir path of a dock.
  QString getDockLaunchersPath(int dockIndex) const {
    return configDir_.filePath(getDockLaunchersDir(dockIndex));
  }

  static QString getDockLaunchersPathForConfigFile(const QString& configFile) {
    QString launcherDir = configFile;
    launcherDir.replace(".conf", "_launchers");
    return launcherDir;
  }

  // Gets the appearance config file path.
  QString getAppearanceConfigPath() const {
    return configDir_.filePath(kAppearanceConfig);
  }

  static QString getWallpaperConfigKey(int desktop) {
    return QString("wallpaper") + QString::number(desktop);
  }

  // Finds the configs of all existing docks.
  std::vector<std::tuple<QString, QString>> findAllDockConfigs();

  // Finds the next available configs for a new dock.
  std::tuple<QString, QString> findNextDockConfigs();

  // Converts the old single-dock config to the new multi-dock config if needed.
  void convertConfig();

 private:
  static void copyEntry(const QString& key, const KConfigGroup& sourceGroup,
                        KConfigGroup* destGroup) {
    destGroup->writeEntry(key, sourceGroup.readEntry(key));
  }

  QDir configDir_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_CONFIG_HELPER_H_
