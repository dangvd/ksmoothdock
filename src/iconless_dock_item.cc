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

#include "iconless_dock_item.h"

namespace ksmoothdock {

int IconlessDockItem::getMaxWidth() const {
  return isHorizontal() ? static_cast<int>(maxSize_ * whRatio_) : maxSize_;
}

int IconlessDockItem::getMaxHeight() const {
  return isHorizontal() ? maxSize_ : static_cast<int>(maxSize_ / whRatio_);
}

int IconlessDockItem::getMinWidth() const {
  return isHorizontal() ? static_cast<int>(minSize_ * whRatio_) : minSize_;
}

int IconlessDockItem::getMinHeight() const {
  return isHorizontal() ? minSize_ : static_cast<int>(minSize_ / whRatio_);
}

int IconlessDockItem::getWidth() const {
  return isHorizontal() ? static_cast<int>(size_ * whRatio_) : size_;
}

int IconlessDockItem::getHeight() const {
  return isHorizontal() ? size_ : static_cast<int>(size_ / whRatio_);
}

}  // namespace ksmoothdock
