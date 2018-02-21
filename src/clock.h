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

#ifndef KSMOOTHDOCK_CLOCK_H_
#define KSMOOTHDOCK_CLOCK_H_

#include "iconless_dock_item.h"

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QString>

#include <KConfig>

#include "calendar.h"

namespace ksmoothdock {

// A digital clock.
class Clock : public QObject, public IconlessDockItem {
  Q_OBJECT

 public:
  Clock(KSmoothDock* parent, Qt::Orientation orientation, int minSize,
        int maxSize, KConfig *config);
  virtual ~Clock() {}

  void draw(QPainter* painter) const override;
  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;
  QString getLabel() const override;

 public slots:
  void updateTime();

  void setDateAndTime();

  void set24HourClock(bool enabled);
  void toggle24HourClock();

 private:
  static constexpr float kWhRatio = 2.8;

  // Creates the context menu.
  void createMenu();

  void saveConfig();

  KConfig* config_;
  bool use24HourClock_;

  QString timeFormat_;

  Calendar calendar_;

  // Context menu.
  QMenu menu_;

  QAction* use24HourClockAction_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_CLOCK_H_
