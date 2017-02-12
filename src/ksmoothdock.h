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

#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>

#include "dock_item.h"

namespace ksmoothdock {

class KSmoothDock : public QMainWindow {
  Q_OBJECT

 public:
  KSmoothDock();
  virtual ~KSmoothDock();

  void init();
  virtual void resize(int w, int h);

 protected:
  virtual void paintEvent(QPaintEvent* e) override;
  virtual void mousePressEvent(QMouseEvent* e) override;

 private:
  const int kDefaultMinSize = 48;
  const int kDefaultMaxSize = 128;

  void loadConfig();
  void loadLaunchers();

  int findActiveItem(int x, int y);

  int desktopWidth_;
  int desktopHeight_;
  int minSize_;
  int maxSize_;
  std::deque<std::unique_ptr<DockItem>> items_;

  Qt::Orientation orientation_;
  bool isMinimized_;
  bool isAnimationActive_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_KSMOOTHDOCK_H_
