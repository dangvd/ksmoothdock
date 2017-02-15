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

#include <cstdlib>
#include <iostream>

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
  setMouseTracking(true);
  desktopWidth_ = QApplication::desktop()->width();
  desktopHeight_ = QApplication::desktop()->height();
  isActivating_ = false;
  animationTimer_.reset(new QTimer(this));
  connect(animationTimer_.get(), SIGNAL(timeout()), this, 
      SLOT(updateAnimation()));
  isAnimationActive_ = false;
}

KSmoothDock::~KSmoothDock() {}

void KSmoothDock::init() {
  loadConfig();
  loadLaunchers();
  initLayoutVars();
  updateLayout();
  KWindowSystem::setStrut(winId(), 0, 0, 0, height());
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
  int minHeight = minSize_ + itemSpacing_;
  painter.fillRect((width() - backgroundWidth_) / 2, height() - minHeight, 
      backgroundWidth_, minHeight, bgColor);

  QColor borderColor("#b1c4de");
  painter.setPen(borderColor);
  painter.drawRect((width() - backgroundWidth_) / 2, height() - minHeight, 
      backgroundWidth_ - 1, minHeight - 1);

  for (int i = 0; i < items_.size(); ++i) {
    items_[i]->draw(&painter);
  }
}

void KSmoothDock::mouseMoveEvent ( QMouseEvent* e) {
  if (isAnimationActive_) {
    return;
  }
  updateLayout(e->x(), e->y());
}

void KSmoothDock::mousePressEvent(QMouseEvent* e) {
  if (isAnimationActive_) {
    return;
  }

  int i = findActiveItem(e->x(), e->y());
  if (i < 0 || i >= items_.size()) {
    return;
  }
  items_[i]->mousePressEvent(e);
}

void KSmoothDock::enterEvent (QEvent* e) {
  isActivating_ = true;
}

void KSmoothDock::leaveEvent(QEvent* e) {
  updateLayout();
}

void KSmoothDock::loadConfig() {
  minSize_ = kDefaultMinSize;
  maxSize_ = kDefaultMaxSize;
  orientation_ = Qt::Horizontal;
  itemSpacing_ = minSize_ / 2;
  parabolicMaxX_ = static_cast<int>(2.5 * (minSize_ + itemSpacing_));
  numAnimationSteps_ = 20;
  animationSpeed_ = 16;
}

void KSmoothDock::loadLaunchers(){
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 0, "Home Folder", orientation_,
      "system-file-manager", minSize_, maxSize_, "dolphin")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 1, "Terminal", orientation_, "utilities-terminal",
      minSize_, maxSize_, "konsole")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 2, "Text Editor", orientation_,
      "accessories-text-editor", minSize_, maxSize_, "kate")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 3, "Web Browser", orientation_,
      "applications-internet", minSize_, maxSize_, "/usr/bin/google-chrome-stable")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 4, "Software Manager", orientation_,
      "system-software-update", minSize_, maxSize_, "mintinstall")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 5, "System Monitor", orientation_,
      "utilities-system-monitor", minSize_, maxSize_, "ksysguard")));
  items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, 6, "System Settings", orientation_,
      "preferences-desktop", minSize_, maxSize_, "systemsettings5")));
}

void KSmoothDock::initLayoutVars() {
  const int distance = minSize_ + itemSpacing_;
  minWidth_ = items_.size() * distance;
  minHeight_ = distance;
  if (items_.size() >= 5) {
    maxWidth_ = parabolic(0) + 2 * parabolic(distance) +
	2 * parabolic(2 * distance) - 5 * minSize_ + minWidth_;
  } else if (items_.size() == 4) {
    maxWidth_ = parabolic(0) + 2 * parabolic(distance) +
	parabolic(2 * distance) - 4 * minSize_ + minWidth_;
  } else if (items_.size() == 3) {
    maxWidth_ = parabolic(0) + 2 * parabolic(distance) -
	3 * minSize_ + minWidth_;
  } else if (items_.size() == 2) {
    maxWidth_ = parabolic(0) + parabolic(distance) -
	2 * minSize_ + minWidth_;
  } else if (items_.size() == 1) {
    maxWidth_ = parabolic(0) - minSize_ + minWidth_;
  }
  maxHeight_ = itemSpacing_ + maxSize_;
}

void KSmoothDock::updateLayout() {
  for (int i = 0; i < items_.size(); ++i) {
    items_[i]->left_ = itemSpacing_ / 2 + i * (minSize_ + itemSpacing_);
    items_[i]->top_ = itemSpacing_ / 2;
    items_[i]->size_ = minSize_;
    items_[i]->minCenter_ = items_[i]->left_ + minSize_ / 2;
  }
  backgroundWidth_ = minWidth_;
  int w = minWidth_;
  int h = minHeight_;
  resize(w, h);
  repaint();
}

void KSmoothDock::updateLayout(int x, int y) {
  if (isActivating_) {
    for (const auto& item : items_) {
      item->startLeft_ = item->left_ + (maxWidth_ - minWidth_) / 2;
      item->startTop_ = item->top_ + (maxHeight_ - minHeight_);
      item->startSize_ = item->size_;
      startBackgroundWidth_ = minWidth_;
    }
  }

  int first_update_index = -1;
  int last_update_index = 0;
  items_[0]->left_ = itemSpacing_ / 2;
  for (int i = 0; i < items_.size(); ++i) {
    int delta = abs(items_[i]->minCenter_ - x + (width() - minWidth_) / 2);
    if (delta < parabolicMaxX_) {
      if (first_update_index == -1) {
        first_update_index = i;
      }
      last_update_index = i;
    }
    items_[i]->size_ = parabolic(delta);
    items_[i]->top_ = itemSpacing_ / 2 + maxSize_ - items_[i]->getHeight();
    if (i > 0) {
      items_[i]->left_ = items_[i - 1]->left_ + items_[i - 1]->getWidth()
	  + itemSpacing_;
    }
  }
  int w;
  for (int i = last_update_index + 1; i < items_.size(); ++i) {
    items_[i]->left_ = maxWidth_
        - (items_.size() - i) * (minSize_ + itemSpacing_) + itemSpacing_ / 2;
  }
  if (first_update_index == 0 && last_update_index < items_.size() - 1) {
    for (int i = last_update_index; i >= first_update_index; --i) {
      items_[i]->left_ = items_[i + 1]->left_ - items_[i]->getWidth()
          - itemSpacing_;
    }
  }
  w = maxWidth_;
  int h = maxHeight_;

  if (isActivating_) {
    for (const auto& item : items_) {
      item->setAnimationEndAsCurrent();
      item->startAnimation(numAnimationSteps_);
      endBackgroundWidth_ = maxWidth_;
      backgroundWidth_ = startBackgroundWidth_;
    }
    currentAnimationStep_ = 0;
    isAnimationActive_ = true;
    isActivating_ = false;
    animationTimer_->start(32 - animationSpeed_);
  }
  resize(w, h);
  repaint();
}

int KSmoothDock::findActiveItem(int x, int y) {
  int i = 0;
  while (i < items_.size() &&
      ((orientation_ == Qt::Horizontal && items_[i]->left_ < x) ||
      (orientation_ == Qt::Vertical && items_[i]->top_ < y))) {
    ++i;
  }
  return i - 1;
}

void KSmoothDock::updateAnimation() {
  for (const auto& item : items_) {
    item->nextAnimationStep();
  }
  ++currentAnimationStep_;
  backgroundWidth_ = startBackgroundWidth_
      + (endBackgroundWidth_ - startBackgroundWidth_)
          * currentAnimationStep_ / numAnimationSteps_;
  if (currentAnimationStep_ == numAnimationSteps_) {
    animationTimer_->stop();
    isAnimationActive_ = false;
  }
  repaint();
}

int ksmoothdock::KSmoothDock::parabolic(int x) {
  // Assume x >= 0.
  if (x > parabolicMaxX_) {
    return minSize_;
  } else {
    return maxSize_ -
        (x * x * (maxSize_ - minSize_)) / (parabolicMaxX_ * parabolicMaxX_);
  }
}

}  // namespace ksmoothdock

#include "ksmoothdock.moc"
