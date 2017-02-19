/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2015 Viet Dang (dangvd@gmail.com)
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

#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QTimer>
#include <QWidget>

#include "dock_item.h"
#include "tooltip.h"

namespace ksmoothdock {

class KSmoothDock : public QWidget {
  Q_OBJECT

 public:
  KSmoothDock();
  virtual ~KSmoothDock();

  void init();

  virtual void resize(int w, int h);

 public slots:
  // Slot to update zoom animation.
  void updateAnimation();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void enterEvent(QEvent* e) override;
  virtual void leaveEvent(QEvent* e) override;

 private:
  const int kDefaultMinSize = 48;
  const int kDefaultMaxSize = 128;

  void loadConfig();
  void loadLaunchers();

  void initMenu();
  void initLayoutVars();

  // Updates width, height, items's size and position when the mouse is outside
  // the dock.
  void updateLayout();

  // Updates width, height, items's size and position given the mouse position.
  void updateLayout(int x, int y);

  // Finds the active item given the mouse position.
  int findActiveItem(int x, int y);

  // Shows tool tip for the item at the specified index.
  void showTooltip(int i);

  // Returns the size given the distance to the mouse.
  int parabolic(int x);

  int desktopWidth_;
  int desktopHeight_;
  int minSize_;
  int maxSize_;
  int itemSpacing_;
  int minWidth_;
  int maxWidth_;
  int minHeight_;
  int maxHeight_;
  int parabolicMaxX_;
  int numAnimationSteps_;
  int animationSpeed_;  
  Qt::Orientation orientation_;

  std::deque<std::unique_ptr<DockItem>> items_;
  std::unique_ptr<QMenu> menu_;
  Tooltip tooltip_;

  bool isEntering_;
  bool isLeaving_;
  bool isAnimationActive_;
  std::unique_ptr<QTimer> animationTimer_;
  int currentAnimationStep_;
  int backgroundWidth_;
  int startBackgroundWidth_;
  int endBackgroundWidth_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_KSMOOTHDOCK_H_
