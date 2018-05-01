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

#ifndef KSMOOTHDOCK_TASK_H_
#define KSMOOTHDOCK_TASK_H_

#include "icon_based_dock_item.h"

#include <memory>

#include <QObject>
#include <QTimer>

#include <KWindowSystem>

#include <model/multi_dock_model.h>

namespace ksmoothdock {

class Task : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  Task(DockPanel* parent, MultiDockModel* model, const QString& label,
       Qt::Orientation orientation, const QPixmap& icon, int minSize,
       int maxSize, WId wId);

  virtual ~Task() = default;

  WId wId() const { return wId_; }

  bool demandAttention() { return demandAttention_; }

  void setDemandAttention(bool value) { demandAttention_ = value; }

  bool active() const { return KWindowSystem::activeWindow() == wId_; }

  void draw(QPainter* painter) const override;

  virtual void mousePressEvent(QMouseEvent* e) override;

 public slots:
  void minimize();

  void maximize();

  void close();

 private:
  MultiDockModel* model_;
  WId wId_;
  bool demandAttention_; // does the task demand attention?

  std::unique_ptr<QTimer> animationTimer_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_LAUNCHER_H_
