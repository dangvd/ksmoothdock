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

#ifndef KSMOOTHDOCK_DESKTOP_SELECTOR_H_
#define KSMOOTHDOCK_DESKTOP_SELECTOR_H_

#include "icon_based_dock_item.h"

#include <KConfig>

namespace ksmoothdock {

class DesktopSelector : public IconBasedDockItem {
 public:
  DesktopSelector(Qt::Orientation orientation, int minSize, int maxSize,
      int desktop, KConfig *config);

  virtual ~DesktopSelector() {}

  virtual void mousePressEvent(QMouseEvent* e) override;

 private:
  // The desktop that this desktop selector manages, 1-based.
  int desktop_;

  KConfig* config_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DESKTOP_SELECTOR_H_
