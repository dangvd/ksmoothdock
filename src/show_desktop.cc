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

#include "show_desktop.h"

#include <memory>

#include <KWindowSystem>

namespace ksmoothdock {

ShowDesktop* ShowDesktop::instance() {
  static std::unique_ptr<ShowDesktop> instance(new ShowDesktop());
  return instance.get();
}

void ShowDesktop::toggleShowDesktop() {
  isShowingDesktop_ = !isShowingDesktop_;
  // KWindowSystem::setShowingDesktop() might leave some boxes
  // in the corners so we need implement our own.
  if (isShowingDesktop_) {
    activeWindow_ = KWindowSystem::activeWindow();
    for (const WId& id : KWindowSystem::stackingOrder()) {
      KWindowSystem::minimizeWindow(id);
      minimizedWindows_.push_back(id);
    }
  } else {
    for (const WId& id : minimizedWindows_) {
      KWindowSystem::unminimizeWindow(id);
    }
    KWindowSystem::activateWindow(activeWindow_);
  }
}

}  // namespace ksmoothdock
