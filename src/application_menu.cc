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

#include "application_menu.h"

#include <KConfigGroup>
#include <KLocalizedString>

namespace ksmoothdock {

ApplicationMenu::ApplicationMenu(
    KSmoothDock *parent, Qt::Orientation orientation, int minSize, int maxSize,
    KConfig *config, const QString &entryDir)
    : IconBasedDockItem(parent, "" /* label */, orientation, "" /* iconName */,
                        minSize, maxSize),
      config_(config),
      entryDir_(entryDir) {
  loadConfig();
  loadMenu();
}

void ApplicationMenu::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    applications_.popup(e->globalPos());
  } else if (e->button() == Qt::RightButton) {
    //menu_.popup(e->globalPos());
  }
}

void ApplicationMenu::loadConfig() {
  KConfigGroup group(config_, "Application Menu");
  setLabel(group.readEntry("label", i18n("Applications")));
  setIconName(group.readEntry("icon", "start-here-kde"));
}

void ApplicationMenu::loadMenu() {
  // TODO
  applications_.addMenu(QIcon::fromTheme("applications-development"),
                   i18n("Development"));
  applications_.addMenu(QIcon::fromTheme("applications-graphics"),
                   i18n("Graphics"));
  applications_.addMenu(QIcon::fromTheme("applications-internet"),
                   i18n("Internet"));
  applications_.addMenu(QIcon::fromTheme("applications-multimedia"),
                   i18n("Multimedia"));
}

}  // namespace ksmoothdock
