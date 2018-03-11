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

#ifndef KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
#define KSMOOTHDOCK_MULTI_DOCK_MODEL_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include <QColor>
#include <QDir>
#include <QObject>
#include <QString>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include "config_helper.h"

namespace ksmoothdock {

constexpr int kDefaultMinSize = 48;
constexpr int kDefaultMaxSize = 128;
constexpr int kDefaultTooltipFontSize = 20;
constexpr float kDefaultBackgroundAlpha = 0.42;
constexpr char kDefaultBackgroundColor[] = "#638abd";
constexpr bool kDefaultShowBorder = true;
constexpr char kDefaultBorderColor[] = "#b1c4de";
constexpr float kLargeFontScaleFactor = 1.0;
constexpr float kMediumFontScaleFactor = 0.8;
constexpr float kSmallFontScaleFactor = 0.6;

constexpr bool kDefaultAutoHide = false;
constexpr bool kDefaultShowApplicationMenu = true;
constexpr bool kDefaultShowPager = false;
constexpr bool kDefaultShowClock = false;

enum class PanelPosition {Top, Bottom, Left, Right};

struct LauncherConfig {
  QString name;
  QString icon;
  QString command;

  LauncherConfig() = default;
  LauncherConfig(const QString& name2, const QString& icon2,
                 const QString& command2)
      : name(name2), icon(icon2), command(command2) {}
  LauncherConfig(const QString& desktopFile);

  // Saves to file in desktop file format.
  void saveToFile(const QString& filePath) const;
};

// The model.
class MultiDockModel : public QObject {
  Q_OBJECT

 public:
  MultiDockModel(const QString& configDir);
  ~MultiDockModel();

  MultiDockModel(const MultiDockModel&) = delete;
  MultiDockModel& operator=(const MultiDockModel&) = delete;

  // Returns the number of docks.
  int dockCount() const { return dockConfigs_.size(); }

  // Adds a new dock in the specified position and screen.
  void addDock(PanelPosition position, int screen);

  // Clones an existing dock in the specified position and screen.
  void cloneDock(int srcDockId, PanelPosition position, int screen);

  // Removes a dock.
  void removeDock(int dockId);

  int minIconSize() const {
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kMinimumIconSize,
          kDefaultMinSize);
  }

  void setMinIconSize(int value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kMinimumIconSize,
                          value);
  }

  int maxIconSize() const {
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kMaximumIconSize,
          kDefaultMaxSize);
  }

  void setMaxIconSize(int value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kMaximumIconSize, value);
  }

  QColor backgroundColor() const {
    QColor defaultBackgroundColor(kDefaultBackgroundColor);
    defaultBackgroundColor.setAlphaF(kDefaultBackgroundAlpha);
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kBackgroundColor,
          defaultBackgroundColor);
  }

  void setBackgroundColor(const QColor& value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kBackgroundColor, value);
  }

  bool showBorder() const {
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kShowBorder,
          kDefaultShowBorder);
  }

  void setShowBorder(bool value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kShowBorder, value);
  }

  QColor borderColor() const {
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kBorderColor,
          QColor(kDefaultBorderColor));
  }

  void setBorderColor(const QColor& value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kBorderColor, value);
  }

  int tooltipFontSize() const {
    return appearanceProperty(
          ConfigHelper::kGeneralCategory, ConfigHelper::kTooltipFontSize,
          kDefaultTooltipFontSize);
  }

  void setTooltipFontSize(int value) {
    setAppearanceProperty(ConfigHelper::kGeneralCategory,
                          ConfigHelper::kTooltipFontSize, value);
  }

  QString applicationMenuName() const {
    return appearanceProperty(ConfigHelper::kApplicationMenuCategory,
                              ConfigHelper::kLabel,
                              i18n("Applications"));
  }

  void setApplicationMenuName(const QString& value) {
    setAppearanceProperty(ConfigHelper::kApplicationMenuCategory,
                          ConfigHelper::kLabel, value);
  }

  QString applicationMenuIcon() const {
    return appearanceProperty(ConfigHelper::kApplicationMenuCategory,
                              ConfigHelper::kIcon,
                              QString("start-here-kde"));
  }

  void setApplicationMenuIcon(const QString& value) {
    setAppearanceProperty(ConfigHelper::kApplicationMenuCategory,
                          ConfigHelper::kIcon, value);
  }

  QString wallpaper(int desktop) const {
    return appearanceProperty(ConfigHelper::kPagerCategory,
                              ConfigHelper::wallpaperConfigKey(desktop),
                              QString());
  }

  void setWallpaper(int desktop, const QString& value) {
    setAppearanceProperty(ConfigHelper::kPagerCategory,
                          ConfigHelper::wallpaperConfigKey(desktop), value);
  }

  bool use24HourClock() const {
    return appearanceProperty(ConfigHelper::kClockCategory,
                              ConfigHelper::kUse24HourClock,
                              true);
  }

  void setUse24HourClock(bool value) {
    setAppearanceProperty(ConfigHelper::kClockCategory,
                          ConfigHelper::kUse24HourClock, value);
  }

  float fontScaleFactor() const {
    return appearanceProperty(ConfigHelper::kClockCategory,
                              ConfigHelper::kFontScaleFactor,
                              kLargeFontScaleFactor);
  }

  void setFontScaleFactor(float value) {
    setAppearanceProperty(ConfigHelper::kClockCategory,
                          ConfigHelper::kFontScaleFactor, value);
  }

  void saveAppearanceConfig() {
    syncAppearanceConfig();
    emit appearanceChanged();
  }

  PanelPosition panelPosition(int dockId) const {
    return static_cast<PanelPosition>(dockProperty(
        dockId, ConfigHelper::kGeneralCategory, ConfigHelper::kPosition,
        static_cast<int>(PanelPosition::Bottom)));
  }

  void setPanelPosition(int dockId, PanelPosition value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kPosition, static_cast<int>(value));
  }

  int screen(int dockId) const {
    return dockProperty(dockId, ConfigHelper::kGeneralCategory,
                        ConfigHelper::kScreen, 0);
  }

  void setScreen(int dockId, int value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kScreen, value);
  }

  bool autoHide(int dockId) const {
    return dockProperty(dockId, ConfigHelper::kGeneralCategory,
                        ConfigHelper::kAutoHide, kDefaultAutoHide);
  }

  void setAutoHide(int dockId, bool value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kAutoHide, value);
  }

  bool showApplicationMenu(int dockId) const {
    return dockProperty(
          dockId, ConfigHelper::kGeneralCategory,
          ConfigHelper::kShowApplicationMenu, kDefaultShowApplicationMenu);
  }

  void setShowApplicationMenu(int dockId, bool value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kShowApplicationMenu, value);
  }

  bool showPager(int dockId) const {
    return dockProperty(
        dockId, ConfigHelper::kGeneralCategory, ConfigHelper::kShowPager,
        kDefaultShowPager);
  }

  void setShowPager(int dockId, bool value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kShowPager, value);
  }

  bool showClock(int dockId) const {
    return dockProperty(
        dockId, ConfigHelper::kGeneralCategory, ConfigHelper::kShowClock,
        kDefaultShowClock);
  }

  void setShowClock(int dockId, bool value) {
    setDockProperty(dockId, ConfigHelper::kGeneralCategory,
                    ConfigHelper::kShowClock, value);
  }

  void saveDockConfig(int dockId) {
    syncDockConfig(dockId);
    // No need to emit signal here.
  }

  std::vector<LauncherConfig> launcherConfigs(int dockId) {
    return std::get<1>(dockConfigs_[dockId]);
  }

  void setLauncherConfigs(int dockId,
                          const std::vector<LauncherConfig>& launcherConfigs) {
    std::get<1>(dockConfigs_[dockId]) = launcherConfigs;
  }

  void saveLauncherConfigs(int dockId) {
    syncLaunchersConfig(dockId);
    emit dockLaunchersChanged(dockId);
  }

 signals:
  void appearanceChanged();
  void dockAdded(int dockId);
  void dockRemoved(int dockId);
  void dockLaunchersChanged(int dockId);

 private:
  template <typename T>
  T appearanceProperty(QString category, QString name, T defaultValue) const {
    KConfigGroup group(&appearanceConfig_, category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setAppearanceProperty(QString category, QString name, T value) {
    KConfigGroup group(&appearanceConfig_, category);
    group.writeEntry(name, value);
  }

  template <typename T>
  T dockProperty(int dockId, QString category, QString name, T defaultValue)
      const {
    KConfigGroup group(dockConfig(dockId), category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setDockProperty(int dockId, QString category, QString name, T value) {
    KConfigGroup group(dockConfig(dockId), category);
    group.writeEntry(name, value);
  }

  const KConfig* dockConfig(int dockId) const {
    return std::get<0>(dockConfigs_.at(dockId)).get();
  }

  KConfig* dockConfig(int dockId) {
    return std::get<0>(dockConfigs_[dockId]).get();
  }

  static std::vector<LauncherConfig> loadDockLaunchers(
      const QString& dockLaunchersPath);

  static std::vector<LauncherConfig> createDefaultLaunchers();

  int addDock(const std::tuple<QString, QString>& configs,
              PanelPosition position, int screen);

  void syncAppearanceConfig() {
    appearanceConfig_.sync();
  }

  void syncDockConfig(int dockId) {
    dockConfig(dockId)->sync();
  }

  void syncLaunchersConfig(int dockId);

  // Helper(s).
  ConfigHelper configHelper_;

  // Model data.
  KConfig appearanceConfig_;
  std::unordered_map<int,
                     std::tuple<std::unique_ptr<KConfig>,
                                std::vector<LauncherConfig>>> dockConfigs_;

  // ID for the next dock.
  int nextDockId_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
