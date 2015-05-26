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

#include "ksmoothdock.h"

#include <KWindowSystem>
#include <netwm_def.h>

#include <QApplication>
#include <QColor>
#include <QDesktopWidget>
#include <QPainter>

#include "launcher.h"

namespace ksmoothdock {

KSmoothDock::KSmoothDock() {
  setAttribute(Qt::WA_TranslucentBackground);
  KWindowSystem::setType(winId(), NET::Dock);
  desktopWidth_ = QApplication::desktop()->width();
  desktopHeight_ = QApplication::desktop()->height();
}

KSmoothDock::~KSmoothDock() {}

void KSmoothDock::init() {
  loadConfig();
  loadLaunchers();

  int w = items_.size() * maxSize_;
  int h = minSize_;
  KWindowSystem::setStrut(winId(), 0, 0, 0, h);
  resize(w, maxSize_);
}

void KSmoothDock::resize(int w, int h) {
  QWidget::resize(w, h);
  int x = (desktopWidth_ - w) / 2;
  move(x, desktopHeight_ - h);
}

void KSmoothDock::paintEvent(QPaintEvent* e) {
  QPainter painter(this);

  QColor bgColor("#638abd");
  bgColor.setAlphaF(0.42);
  painter.fillRect(0, height() - minSize_, width(), minSize_, bgColor);

  QColor borderColor("#b1c4de");
  painter.setPen(borderColor);
  painter.drawRect(0, height() - minSize_, width() - 1, minSize_ - 1);

  for (int i = 0; i < items_.size(); ++i) {
    items_[i]->draw(&painter, i * maxSize_, 0, maxSize_);
  }
}

void KSmoothDock::loadConfig() {
  minSize_ = kDefaultMinSize;
  maxSize_ = kDefaultMaxSize;
}

void KSmoothDock::loadLaunchers(){
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 0, "Terminal", Qt::Horizontal, "utilities-terminal",
      minSize_, maxSize_, "konsole")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 1, "Web Browser", Qt::Horizontal,
      "applications-internet", minSize_, maxSize_, "chromium-browser")));
}

}  // namespace ksmoothdock

#include "ksmoothdock.moc"
