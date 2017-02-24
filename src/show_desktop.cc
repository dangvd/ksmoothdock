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
      KWindowInfo info(id,  NET::XAWMState);
      if (info.mappingState() == NET::Visible) {
        KWindowSystem::minimizeWindow(id);
        minimizedWindows_.push_back(id);
      }
    }
  } else {
    for (const WId& id : minimizedWindows_) {
      KWindowSystem::unminimizeWindow(id);
    }
    minimizedWindows_.clear();
    KWindowSystem::activateWindow(activeWindow_);
  }
}

void ShowDesktop::reset() {
  if (isShowingDesktop_) {
    isShowingDesktop_ = false;
    minimizedWindows_.clear();
  }
}

void ShowDesktop::activeWindowChanged(WId id) {
  KWindowInfo info(id, NET::WMWindowType);
  const auto& type = info.windowType(NET::NormalMask | NET::DialogMask);
  if (type == NET::Normal || type == NET::Dialog) {
    reset();
  }
}

ShowDesktop::ShowDesktop() : QObject(),
    isShowingDesktop_(false),
    minimizedWindows_(),
    activeWindow_(0) {
  connect(KWindowSystem::self(), SIGNAL(currentDesktopChanged(int)),
      this, SLOT(reset()));
  connect(KWindowSystem::self(), SIGNAL(activeWindowChanged(WId)),
      this, SLOT(activeWindowChanged(WId)));
}

}  // namespace ksmoothdock
