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

#include "iconbaseddockitem.h"

#include <KIconLoader>

#include <QImage>

namespace ksmoothdock {

IconBasedDockItem::IconBasedDockItem(KSmoothDock* parent, int itemId,
    QString label, Qt::Orientation orientation, QString iconName, int minSize,
    int maxSize)
    : DockItem(parent, itemId, label, orientation), minSize_(minSize), 
      maxSize_(maxSize), icons_(maxSize - minSize + 1) {
  QPixmap icon = KIconLoader::global()->loadIcon(iconName,
      KIconLoader::NoGroup, kIconLoadSize);
  generateIcons(icon);
}

IconBasedDockItem::IconBasedDockItem(KSmoothDock* parent, int itemId, 
    QString label, Qt::Orientation orientation, const QPixmap& icon,
    int minSize, int maxSize)
    : DockItem(parent, itemId, label, orientation), minSize_(minSize), 
      maxSize_(maxSize), icons_(maxSize - minSize + 1) {
  generateIcons(icon);
}

void IconBasedDockItem::setIcon(const QPixmap& icon) {
  icons_.clear();
  // Just in case clear() affects capacity.
  icons_.reserve(maxSize_ - minSize_ + 1);
  generateIcons(icon);
}

void IconBasedDockItem::draw(QPainter* painter, int x, int y, int size) const {
  if (size >= minSize_ && size <= maxSize_) {
    painter->drawPixmap(x, y, icons_[size - minSize_]);
  }
}

int IconBasedDockItem::getMaxWidth() const {
  return getIcon(maxSize_).width();
}

int IconBasedDockItem::getMaxHeight() const {
  return getIcon(maxSize_).height();
}

int IconBasedDockItem::getMinWidth() const {
  return getIcon(minSize_).width();
}

int IconBasedDockItem::getMinHeight() const {
  return getIcon(minSize_).height();
}

int IconBasedDockItem::getWidth(int size) const {
  return getIcon(size).width();
}

int IconBasedDockItem::getHeight(int size) const {
  return getIcon(size).height();
}

void IconBasedDockItem::generateIcons(const QPixmap& icon) {
  QImage image = icon.toImage(); // Convert to QImage for fast scaling.
  for (int size = minSize_; size <= maxSize_; ++size) {
    icons_[size - minSize_] = QPixmap::fromImage(
        (orientation_ == Qt::Horizontal)
            ? image.scaledToHeight(size, Qt::SmoothTransformation)
            : image.scaledToWidth(size, Qt::SmoothTransformation));
  }
}

const QPixmap& IconBasedDockItem::getIcon(int size) const {
  if (size < minSize_) {
    size = minSize_;
  } else if (size > maxSize_) {
    size = maxSize_;
  }
  return icons_[size - minSize_];
}

}  // namespace ksmoothdock
