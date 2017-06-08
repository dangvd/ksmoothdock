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

#ifndef KSMOOTHDOCK_ICONLESS_DOCK_ITEM_H_
#define KSMOOTHDOCK_ICONLESS_DOCK_ITEM_H_

#include "dock_item.h"

namespace ksmoothdock {

// Base class for dock items without an icon, such as clock.
class IconlessDockItem : public DockItem {
 public:
  IconlessDockItem(KSmoothDock* parent, const QString& label,
      Qt::Orientation orientation, int minSize, int maxSize, float whRatio)
      : DockItem(parent, label, orientation, minSize, maxSize),
        whRatio_(whRatio) {}
  virtual ~IconlessDockItem() {}

  int getWidthForSize(int size) const override;
  int getHeightForSize(int size) const override;

 protected:
  // Width/height ratio.
  float whRatio_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_ICONLESS_DOCK_ITEM_H_
