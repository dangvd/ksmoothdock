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

#include "desktop_selector.h"

#include <QPixmap>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KWindowSystem>

namespace ksmoothdock {

DesktopSelector::DesktopSelector(Qt::Orientation orientation, int minSize,
    int maxSize, int desktop, KConfig *config)
    : IconBasedDockItem(
          i18n((QString("Desktop ") + QString::number(desktop))
              .toStdString().c_str()),
          orientation, "" /* no icon yet */, minSize, maxSize),
      desktop_(desktop),
      config_(config) {
  KConfigGroup group(config_, "Pager");
  const QString key = QString("wallpaper") + QString::number(desktop);
  QString wallpaper = group.readEntry(key, "");
  if (!wallpaper.isEmpty()) {
    setIcon(QPixmap(wallpaper));
  } else {
    setIconName("user-desktop");
  }
}

void DesktopSelector::mousePressEvent(QMouseEvent* e) {
  if (e->button() == Qt::LeftButton) {
    if (KWindowSystem::currentDesktop() != desktop_) {
      KWindowSystem::setCurrentDesktop(desktop_);
    } else {
      KWindowSystem::setShowingDesktop(!KWindowSystem::showingDesktop());
    }
  }
}

}  // namespace ksmoothdock
