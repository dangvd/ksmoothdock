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

#ifndef KSMOOTHDOCK_DOCK_PANEL_H_
#define KSMOOTHDOCK_DOCK_PANEL_H_

#include <memory>
#include <vector>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QTimer>
#include <QWidget>

#include <KAboutApplicationDialog>
#include <KWindowSystem>

#include "appearance_settings_dialog.h"
#include "dock_item.h"
#include "edit_launchers_dialog.h"
#include "tooltip.h"

namespace ksmoothdock {

class MultiDockView;

// A dock panel. The user can have multiple dock panels at the same time.
class DockPanel : public QWidget {
  Q_OBJECT

 public:
  // No pointer ownership.
  DockPanel(MultiDockView* parent, MultiDockModel* model, int dockId);
  virtual ~DockPanel() = default;

  void resize(int w, int h);

  bool showBorder() { return showBorder_; }
  QColor borderColor() { return borderColor_; }
  QColor backgroundColor() { return backgroundColor_; }

  // Gets the position to show the application menu.
  QPoint applicationMenuPosition(const QSize& menuSize);
  // Gets the position to show the application menu's sub-menus.
  QPoint applicationSubMenuPosition(const QSize& menuSize,
                                       const QRect& subMenuGeometry);

 public slots:
  // Reloads the items and updates the dock.
  void reload();

  void onDockLaunchersChanged(int dockId) {
    if (dockId_ == dockId) {
      reload();
    }
  }

  void setStrut();
  void setStrutForApplicationMenu();

  void updatePosition(PanelPosition position) {
    setPosition(position);
    reload();
    saveDockConfig();
  }

  void updateVisibility(PanelVisibility visibility) {
    setVisibility(visibility);
    reload();
    saveDockConfig();
  }

  void toggleApplicationMenu() {
    showApplicationMenu_ = !showApplicationMenu_;
    applicationMenuSettings_->setVisible(showApplicationMenu_);
    reload();
    saveDockConfig();
  }

  void togglePager();

  void updatePager() {
    if (showPager_) {
      reload();
    }
  }

  void toggleClock() {
    showClock_ = !showClock_;
    reload();
    saveDockConfig();
  }

  // Sets the dock on a specific screen given screen index.
  // Thus 0 is screen 1 and so on.
  // This doesn't refresh the dock.
  void setScreen(int screen);

  // Slot to update zoom animation.
  void updateAnimation();

  void resetCursor();

  void showOnlineDocumentation();

  void about();

  // These are for global appearance settings.
  // Dock-specific settings are activated from menu items on the context menu
  // directly.
  void showAppearanceSettingsDialog();
  void showEditLaunchersDialog();
  void showApplicationMenuSettingsDialog();

  void addDock();
  void cloneDock();
  void removeDock();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void enterEvent(QEvent* e) override;
  virtual void leaveEvent(QEvent* e) override;

 private:
  // The space between the tooltip and the dock.
  static const int kTooltipSpacing = 10;

  void init();

  bool isHorizontal() { return orientation_ == Qt::Horizontal; }

  bool autoHide() { return visibility_ == PanelVisibility::AutoHide; }

  void setPosition(PanelPosition position);

  void setVisibility(PanelVisibility visibility);

  int itemCount() { return static_cast<int>(items_.size()); }

  void reserveItems(int numLaunchers) {
    const int numPagerIcons = showPager_ ? KWindowSystem::numberOfDesktops()
                                         : 0;
    const int numClockIcons = showClock_ ? 1 : 0;
    items_.reserve(numLaunchers + numPagerIcons + numClockIcons);
  }

  void initUi();

  void createMenu();

  void loadDockConfig();
  void saveDockConfig();
  void loadAppearanceConfig();

  void initLaunchers();
  void initApplicationMenu();
  void initPager();
  void initClock();

  void initLayoutVars();

  // Updates width, height, items's size and position when the mouse is outside
  // the dock.
  void updateLayout();

  // Updates width, height, items's size and position given the mouse position.
  void updateLayout(int x, int y);

  void setStrut(int width);

  // Finds the active item given the mouse position.
  int findActiveItem(int x, int y);

  // Shows the appropriate tooltip given the mouse position.
  void showTooltip(int x, int y);
  // Shows tool tip for the item at the specified index.
  void showTooltip(int i);

  void showWaitCursor();

  void showPagerInfoDialog();

  // Returns the size given the distance to the mouse.
  int parabolic(int x);

  MultiDockView* parent_;

  // The model.
  MultiDockModel* model_;
  int dockId_;

  // Config variables.

  PanelPosition position_;
  int screen_;  // the screen (as screen index) that the dock is on.
  PanelVisibility visibility_;
  bool showApplicationMenu_;
  bool showPager_;
  bool showClock_;
  int minSize_;
  int maxSize_;
  QColor backgroundColor_;  // including alpha.
  bool showBorder_;
  QColor borderColor_;  // no alpha.
  int tooltipFontSize_;

  // Non-config variables.

  int itemSpacing_;
  int minX_;  // X-coordinate when minimized.
  int minY_;  // Y-coordinate when minimized.
  int minWidth_;
  int maxWidth_;
  int minHeight_;
  int maxHeight_;
  int parabolicMaxX_;
  QRect screenGeometry_;  // the geometry of the screen that the dock is on.

  // Number of animation steps when zooming in and out.
  int numAnimationSteps_;
  // Animation speed, between 0 and 31. The higher the faster.
  int animationSpeed_;

  Qt::Orientation orientation_;

  // The list of all dock items.
  std::vector<std::unique_ptr<DockItem>> items_;

  // Context (right-click) menu.
  QMenu menu_;
  QAction* positionTop_;
  QAction* positionBottom_;
  QAction* positionLeft_;
  QAction* positionRight_;
  QAction* visibilityAlwaysVisibleAction_;
  QAction* visibilityAutoHideAction_;
  QAction* applicationMenuAction_;
  QAction* applicationMenuSettings_;
  QAction* pagerAction_;
  QAction* clockAction_;
  // Actions to set the dock on a specific screen.
  std::vector<QAction*> screenActions_;

  KAboutApplicationDialog aboutDialog_;
  AppearanceSettingsDialog appearanceSettingsDialog_;
  EditLaunchersDialog editLaunchersDialog_;

  // The tooltip object to show tooltip for the active item.
  Tooltip tooltip_;

  bool isMinimized_;
  bool isResizing_;
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

  // For recording the mouse position before doing entering animation
  // so that we can show the correct tooltip at the end of it.
  int mouseX_;
  int mouseY_;

  friend class DockPanelTest;
  friend class ConfigDialogTest;
  friend class EditLaunchersDialogTest;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_PANEL_H_
