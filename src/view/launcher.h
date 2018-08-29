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

#ifndef KSMOOTHDOCK_LAUNCHER_H_
#define KSMOOTHDOCK_LAUNCHER_H_

#include "icon_based_dock_item.h"

#include <model/multi_dock_model.h>
#include <utils/command_utils.h>

namespace ksmoothdock {

class Launcher : public IconBasedDockItem {
 public:
  Launcher(DockPanel* parent, MultiDockModel* model, const QString& label,
      Qt::Orientation orientation, const QString& iconName, int minSize,
      int maxSize, const QString& command);

  virtual ~Launcher() = default;

  QString command() const { return command_; }

  void setLaunching(bool launching) { launching_ = launching; }

  void draw(QPainter* painter) const override;

  void mousePressEvent(QMouseEvent* e) override;

  static void launch(const QString& command);
  static void lockScreen() { launch(kLockScreenCommand); }

 private:
  MultiDockModel* model_;
  QString command_;
  bool launching_;

  friend class DockPanel;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_LAUNCHER_H_
