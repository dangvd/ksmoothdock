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

#ifndef KSMOOTHDOCK_KSMOOTHDOCK_H_
#define KSMOOTHDOCK_KSMOOTHDOCK_H_

#include <deque>
#include <memory>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QString>
#include <QTimer>
#include <QWidget>

#include <KAboutApplicationDialog>
#include <KConfig>

#include "config_dialog.h"
#include "dock_item.h"
#include "edit_launchers_dialog.h"
#include "tooltip.h"

namespace ksmoothdock {

enum class PanelPosition {Top, Bottom, Left, Right};

class KSmoothDock : public QWidget {
  Q_OBJECT

 public:
  KSmoothDock(const QString& configPath, const QString& launchersPath);
  KSmoothDock();

  virtual ~KSmoothDock();

  void init();

  virtual void resize(int w, int h);

  // For testing only.
  ConfigDialog* configDialog() { return &configDialog_; }

 public slots:
  // Opens launchers directory in the file manager.
  void openLaunchersDir();

  void reload();
  void refresh();

  void setPosition(PanelPosition position);

  void setPositionTop() {
    setPosition(PanelPosition::Top);
    reload(); 
    saveConfig();
  }

  void setPositionBottom() {
    setPosition(PanelPosition::Bottom);
    reload();
    saveConfig();
  }

  void setPositionLeft() {
    setPosition(PanelPosition::Left);
    reload();
    saveConfig();
  }

  void setPositionRight() {
    setPosition(PanelPosition::Right);
    reload();
    saveConfig();
  }

  void toggleAutoHide() {
    autoHide_ = !autoHide_;
    reload();
    saveConfig();
  }

  void togglePager() {
    showPager_ = !showPager_;
    reload();
    saveConfig();
  }

  void updatePager() {
    if (showPager_) {
      reload();
    }
  }

  // Slot to update zoom animation.
  void updateAnimation();

  void resetCursor();

  void about();

  void showConfigDialog();
  void applyConfig();
  void updateConfig();
  void resetConfig();

  void showEditLaunchersDialog();
  void applyLauncherConfig();
  void updateLauncherConfig();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void enterEvent(QEvent* e) override;
  virtual void leaveEvent(QEvent* e) override;

 private:
  static const int kDefaultMinSize = 48;
  static const int kDefaultMaxSize = 128;
  static const int kDefaultTooltipFontSize = 20;
  // The space between the tooltip and the dock.
  static const int kTooltipSpacing = 10;

  static const char kDefaultBackgroundColor[];
  static const char kDefaultBorderColor[];
  static const float kBackgroundAlpha;

  bool isHorizontal() { return orientation_ == Qt::Horizontal; }

  int numItems() { return static_cast<int>(items_.size()); }

  void createMenu();

  void loadConfig();
  void saveConfig();

  void initLaunchers();
  bool loadLaunchers();
  void createDefaultLaunchers();
  void saveLaunchers();

  void initPager();

  void initLayoutVars();

  // Updates width, height, items's size and position when the mouse is outside
  // the dock.
  void updateLayout();

  // Updates width, height, items's size and position given the mouse position.
  void updateLayout(int x, int y);

  void setStrut();

  // Finds the active item given the mouse position.
  int findActiveItem(int x, int y);

  // Shows tool tip for the item at the specified index.
  void showTooltip(int i);

  void showWaitCursor();

  // Returns the size given the distance to the mouse.
  int parabolic(int x);

  // Config variables.

  PanelPosition position_;
  bool autoHide_;
  bool showPager_;
  int minSize_;
  int maxSize_;
  QColor backgroundColor_;
  QColor borderColor_;
  int tooltipFontSize_;

  // Non-config variables.

  int itemSpacing_;
  int minWidth_;
  int maxWidth_;
  int minHeight_;
  int maxHeight_;
  int parabolicMaxX_;
  int numAnimationSteps_;
  int animationSpeed_;

  Qt::Orientation orientation_;
  int desktopWidth_;
  int desktopHeight_;

  // This is a relative path to the home dir.
  QString launchersRelativePath_;
  // The path to the directory to store quick launchers
  // as desktop files.
  QString launchersPath_;

  QString configPath_;
  KConfig config_;

  // The list of all dock items.
  std::deque<std::unique_ptr<DockItem>> items_;

  // Context (right-click) menu.
  QMenu menu_;
  QAction* positionTop_;
  QAction* positionBottom_;
  QAction* positionLeft_;
  QAction* positionRight_;
  QAction* autoHideAction_;
  QAction* pagerAction_;

  KAboutApplicationDialog aboutDialog_;
  ConfigDialog configDialog_;
  EditLaunchersDialog editLaunchersDialog_;

  // The tooltip object to show tooltip for the active item.
  Tooltip tooltip_;

  bool isEntering_;
  bool isLeaving_;
  bool isAnimationActive_;
  std::unique_ptr<QTimer> animationTimer_;
  int currentAnimationStep_;
  int backgroundWidth_;
  int startBackgroundWidth_;
  int endBackgroundWidth_;
  int backgroundHeight_;
  int startBackgroundHeight_;
  int endBackgroundHeight_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_KSMOOTHDOCK_H_
