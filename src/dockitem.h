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

#ifndef KSMOOTHDOCK_DOCK_ITEM_H_
#define KSMOOTHDOCK_DOCK_ITEM_H_

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <Qt>

namespace ksmoothdock {

class KSmoothDock;

// Base class for all dock items, i.e. launchers, tasks, clock etc.
class DockItem {
 public:
  const static int kAllDesktops = 0;

  DockItem(KSmoothDock* parent, int itemId, QString label, 
      Qt::Orientation orientation, int desktop = kAllDesktops)
      : parent_(parent), itemId_(itemId), label_(label),
        orientation_(orientation), desktop_(desktop) {}
  virtual ~DockItem() {}

  int getDesktop() { return desktop_; }

  int getItemId() { return itemId_; }

  // This is virtual so dynamic label can be implemented in its subclasses.
  virtual QString getLabel() { return label_; }
  void setLabel(QString label) { label = label_; }

  // Draws itself on the parent's canvas.
  virtual void draw(QPainter* painter, int x, int y, int size) const = 0;

  virtual void mousePressEvent(QMouseEvent* e) const = 0;

  // Gets max width, i.e. the width when the item is max zoomed.
  virtual int getMaxWidth() const = 0;

  // Gets max height, i.e. the width when the item is max zoomed.
  virtual int getMaxHeight() const = 0;

  // Gets min width, i.e. when the item is not zoomed in.
  virtual int getMinWidth() const = 0;

  /// Gets min height, i.e. when the item is not zoomed in.
  virtual int getMinHeight() const = 0;

  /// Gets width for a specific size.
  virtual int getWidth(int size) const = 0;
  
  /// Gets height for a specific size.
  virtual int getHeight(int size) const = 0;

protected:
  KSmoothDock* parent_; // The dock parent.
  int itemId_; // Unique item ID.
  QString label_; // Label of the dock item.
  int desktop_; // The virtual desktop this dock item is on.
  Qt::Orientation orientation_; // Orientation (horizontal/vertical).
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_ITEM_H_
