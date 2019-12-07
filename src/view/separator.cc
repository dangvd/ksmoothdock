/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2019 Viet Dang (dangvd@gmail.com)
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

#include "separator.h"

namespace ksmoothdock {

Separator::Separator(DockPanel* parent, MultiDockModel* model, Qt::Orientation orientation,
                     int minSize, int maxSize)
    : IconlessDockItem(parent, "" /* label */, orientation, minSize, maxSize, kWhRatio),
      model_(model) {}

void Separator::draw(QPainter* painter) const {

}

void Separator::mousePressEvent(QMouseEvent* e) {

}

void Separator::createMenu() {

}

}  // namespace ksmoothdock
