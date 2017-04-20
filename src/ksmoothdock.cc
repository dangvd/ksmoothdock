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

#include "ksmoothdock.h"

#include <cstdlib>
#include <iostream>
#include <utility>

#include <QApplication>
#include <QColor>
#include <QCursor>
#include <QDir>
#include <QDesktopWidget>
#include <QIcon>
#include <QListWidgetItem>
#include <QPainter>
#include <QProcess>
#include <QStringList>
#include <QVariant>

#include <KAboutData>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KWindowSystem>
#include <netwm_def.h>

#include "desktop_selector.h"
#include "launcher.h"
#include "welcome_dialog.h"

namespace ksmoothdock {

const int KSmoothDock::kDefaultMinSize;
const int KSmoothDock::kDefaultMaxSize;
const int KSmoothDock::kDefaultTooltipFontSize;
const int KSmoothDock::kTooltipSpacing;
const float KSmoothDock::kBackgroundAlpha = 0.42;
const char KSmoothDock::kDefaultBackgroundColor[] = "#638abd";
const char KSmoothDock::kDefaultBorderColor[] = "#b1c4de";

KSmoothDock::KSmoothDock(const QString& configFile,
                         const QString& launchersDir)
    : QWidget(),
      autoHide_(false),
      showPager_(false),
      launchersDir_(launchersDir),
      configFile_(configFile),
      config_(configFile, KConfig::SimpleConfig),
      aboutDialog_(KAboutData::applicationData(), this),
      configDialog_(this),
      editLaunchersDialog_(this),
      isEntering_(false),
      isLeaving_(false),
      isAnimationActive_(false) {}

KSmoothDock::KSmoothDock()
    : KSmoothDock(QDir::homePath() + "/.ksmoothdock/ksmoothdockrc",
                  QDir::homePath() + "/.ksmoothdock/launchers") {}

void KSmoothDock::init() {
  setAttribute(Qt::WA_TranslucentBackground);
  KWindowSystem::setType(winId(), NET::Dock);
  KWindowSystem::setOnAllDesktops(winId(), true);
  setMouseTracking(true);
  desktopWidth_ = QApplication::desktop()->screenGeometry().width();
  desktopHeight_ = QApplication::desktop()->screenGeometry().height();
  animationTimer_.reset(new QTimer(this));
  connect(animationTimer_.get(), SIGNAL(timeout()), this,
      SLOT(updateAnimation()));
  connect(KWindowSystem::self(), SIGNAL(numberOfDesktopsChanged(int)),
      this, SLOT(updatePager()));
  createMenu();
  loadConfig();

  initUi();
}

KSmoothDock::~KSmoothDock() {
  saveConfig();
}

void KSmoothDock::resize(int w, int h) {
  QWidget::resize(w, h);
  int x, y;
  if (position_ == PanelPosition::Top) {
    x = (desktopWidth_ - w) / 2;
    y = 0;
  } else if (position_ == PanelPosition::Bottom) {
    x = (desktopWidth_ - w) / 2;
    y = desktopHeight_ - h;
  } else if (position_ == PanelPosition::Left) {
    x = 0;
    y = (desktopHeight_ - h) / 2;
  } else {  // Right
    x = desktopWidth_ - w;
    y = (desktopHeight_ - h) / 2;
  }
  move(x, y);
  // This is to fix the bug that if launched from Plasma Quicklaunch,
  // KSmoothDock still doesn't show on all desktops even though
  // we've already called this in the constructor.
  KWindowSystem::setOnAllDesktops(winId(), true);
}

void KSmoothDock::openLaunchersDir() {
  Launcher::launch("dolphin " + launchersDir_);
}

void KSmoothDock::reload() {
  items_.clear();
  initUi();
  repaint();
}

void KSmoothDock::refresh() {
  initLayoutVars();
  updateLayout();
  repaint();
}

void KSmoothDock::setPosition(PanelPosition position) {
  position_ = position;
  orientation_ = (position_ == PanelPosition::Top ||
      position_ == PanelPosition::Bottom)
      ? Qt::Horizontal : Qt::Vertical;
  positionTop_->setChecked(position == PanelPosition::Top);
  positionBottom_->setChecked(position == PanelPosition::Bottom);
  positionLeft_->setChecked(position == PanelPosition::Left);
  positionRight_->setChecked(position == PanelPosition::Right);
}

void KSmoothDock::updateAnimation() {
  for (const auto& item : items_) {
    item->nextAnimationStep();
  }
  ++currentAnimationStep_;
  backgroundWidth_ = startBackgroundWidth_
      + (endBackgroundWidth_ - startBackgroundWidth_)
          * currentAnimationStep_ / numAnimationSteps_;
  backgroundHeight_ = startBackgroundHeight_
      + (endBackgroundHeight_ - startBackgroundHeight_)
          * currentAnimationStep_ / numAnimationSteps_;
  if (currentAnimationStep_ == numAnimationSteps_) {
    animationTimer_->stop();
    isAnimationActive_ = false;
    if (isLeaving_) {
      isLeaving_ = false;
      updateLayout();
    }
  }
  repaint();
}

void KSmoothDock::resetCursor() {
  setCursor(QCursor(Qt::ArrowCursor));
}

void KSmoothDock::about() {
  aboutDialog_.show();
}

void KSmoothDock::showConfigDialog() {
  configDialog_.minSize_->setValue(minSize_);
  configDialog_.maxSize_->setValue(maxSize_);
  configDialog_.backgroundColor_->setColor(QColor(backgroundColor_.rgb()));
  configDialog_.borderColor_->setColor(borderColor_);
  configDialog_.tooltipFontSize_->setValue(tooltipFontSize_);
  configDialog_.show();
}

void KSmoothDock::applyConfig() {
  minSize_ = configDialog_.minSize_->value();
  if (configDialog_.maxSize_->value() < minSize_) {
    configDialog_.maxSize_->setValue(minSize_);
  }
  maxSize_ = configDialog_.maxSize_->value();
  backgroundColor_ = configDialog_.backgroundColor_->color();
  backgroundColor_.setAlphaF(kBackgroundAlpha);
  borderColor_ = configDialog_.borderColor_->color();
  tooltipFontSize_ = configDialog_.tooltipFontSize_->value();

  saveConfig();
  reload();
}

void KSmoothDock::updateConfig() {
  applyConfig();
  configDialog_.hide();
}

void KSmoothDock::resetConfig() {
  configDialog_.minSize_->setValue(kDefaultMinSize);
  configDialog_.maxSize_->setValue(kDefaultMaxSize);
  configDialog_.backgroundColor_->setColor(QColor(kDefaultBackgroundColor));
  configDialog_.borderColor_->setColor(QColor(kDefaultBorderColor));
  configDialog_.tooltipFontSize_->setValue(kDefaultTooltipFontSize);  
}

void KSmoothDock::showEditLaunchersDialog() {
  editLaunchersDialog_.launchers_->clear();
  for (const auto& item : items_) {
    Launcher* launcher = dynamic_cast<Launcher*>(item.get());
    if (launcher != nullptr) {
      QListWidgetItem* listItem = new QListWidgetItem(
          QIcon(launcher->getIcon(kListIconSize)), launcher->label_);
          listItem->setData(Qt::UserRole, QVariant::fromValue(
              LauncherInfo(launcher->iconName_, launcher->command_)));
      editLaunchersDialog_.launchers_->addItem(listItem);
    }
  }
  editLaunchersDialog_.launchers_->setCurrentRow(0);
  editLaunchersDialog_.show();
}

void KSmoothDock::applyLauncherConfig() {
  items_.clear();
  const int numLaunchers = editLaunchersDialog_.launchers_->count();
  items_.reserve(showPager_ ? numLaunchers + KWindowSystem::numberOfDesktops()
                            : numLaunchers);
  for (int i = 0; i < numLaunchers; ++i) {
    QListWidgetItem* listItem = editLaunchersDialog_.launchers_->item(i);
    LauncherInfo info = listItem->data(Qt::UserRole).value<LauncherInfo>();
    items_.push_back(std::unique_ptr<DockItem>(
        new Launcher(this, listItem->text(), orientation_, info.iconName,
            minSize_, maxSize_, info.command)));
  }
  saveLaunchers();

  initPager();
  initLayoutVars();
  updateLayout();
  repaint();
}

void KSmoothDock::updateLauncherConfig() {
  applyLauncherConfig();
  editLaunchersDialog_.close();
}

void KSmoothDock::paintEvent(QPaintEvent* e) {
  QPainter painter(this);

  if (isHorizontal()) {
    const int y = (position_ == PanelPosition::Top) ? 0
        : height() - backgroundHeight_;
    painter.fillRect((width() - backgroundWidth_) / 2, y, 
        backgroundWidth_, backgroundHeight_, backgroundColor_);

    painter.setPen(borderColor_);
    painter.drawRect((width() - backgroundWidth_) / 2, y, 
        backgroundWidth_ - 1, backgroundHeight_ - 1);
  } else {  // Vertical
    const int x =  (position_ == PanelPosition::Left) ? 0
        : width() - backgroundWidth_;
    painter.fillRect(x, (height() - backgroundHeight_) / 2,
        backgroundWidth_, backgroundHeight_, backgroundColor_);

    painter.setPen(borderColor_);
    painter.drawRect(x, (height() - backgroundHeight_) / 2,
        backgroundWidth_ - 1, backgroundHeight_ - 1);
  }

  for (int i = numItems() - 1; i >= 0; --i) {
    items_[i]->draw(&painter);
  }
}

void KSmoothDock::mouseMoveEvent ( QMouseEvent* e) {
  if (isAnimationActive_) {
    return;
  }

  int i = findActiveItem(e->x(), e->y());
  if (i < 0 || i >= numItems()) {
    tooltip_.hide();
  } else {
    showTooltip(i);
    // Somehow we have to set this property again when re-showing.
    KWindowSystem::setOnAllDesktops(tooltip_.winId(), true);
  }

  updateLayout(e->x(), e->y());
}

void KSmoothDock::mousePressEvent(QMouseEvent* e) {
  if (isAnimationActive_) {
    return;
  }

  int i = findActiveItem(e->x(), e->y());
  if (i < 0 || i >= numItems()) {
    return;
  }

  Launcher* launcher = dynamic_cast<Launcher*>(items_[i].get());
  if (e->button() == Qt::LeftButton) {
    if (launcher != nullptr && !launcher->isCommandInternal()
        && !launcher->isCommandDBus()) {
      // Acknowledge launching the program.
      showWaitCursor();
    }
    items_[i]->mousePressEvent(e);
  } else if (e->button() == Qt::RightButton) {
    if (launcher != nullptr) {
      menu_.popup(e->globalPos());
    } else {
      items_[i]->mousePressEvent(e);
    }
  }
}

void KSmoothDock::enterEvent (QEvent* e) {
  isEntering_ = true;
}

void KSmoothDock::leaveEvent(QEvent* e) {
  isLeaving_ = true;
  updateLayout();
  tooltip_.hide();
}

void KSmoothDock::initUi() {
  initLaunchers();
  initPager();
  initLayoutVars();
  updateLayout();
  setStrut();
}

void KSmoothDock::createMenu() {
  menu_.addAction(QIcon::fromTheme("configure"), i18n("Edit &Launchers"), this,
      SLOT(showEditLaunchersDialog()));
  menu_.addAction(QIcon::fromTheme("configure"), i18n("&Settings"), this,
      SLOT(showConfigDialog()));

  QMenu* position = menu_.addMenu(i18n("&Position"));
  positionTop_ = position->addAction(i18n("&Top"), this,
      SLOT(setPositionTop()));
  positionTop_->setCheckable(true);
  positionBottom_ = position->addAction(i18n("&Bottom"), this,
      SLOT(setPositionBottom()));
  positionBottom_->setCheckable(true);
  positionLeft_ = position->addAction(i18n("&Left"), this,
      SLOT(setPositionLeft()));
  positionLeft_->setCheckable(true);
  positionRight_ = position->addAction(i18n("&Right"), this,
      SLOT(setPositionRight()));
  positionRight_->setCheckable(true);

  autoHideAction_ = menu_.addAction(i18n("Auto &Hide"), this,
      SLOT(toggleAutoHide()));
  autoHideAction_->setCheckable(true);

  QMenu* extraComponents = menu_.addMenu(i18n("&Extra Components"));
  pagerAction_ = extraComponents->addAction(i18n("Pager"), this,
      SLOT(togglePager()));
  pagerAction_->setCheckable(true);

  menu_.addSeparator();
  menu_.addAction(QIcon::fromTheme("help-about"), i18n("&About KSmoothDock"),
      this, SLOT(about()));
  menu_.addSeparator();
  menu_.addAction(i18n("E&xit"), this, SLOT(close()));
}

void KSmoothDock::loadConfig() {
  KConfigGroup group(&config_, "General");

  PanelPosition position;
  const bool firstRun = !QFile(configFile_).exists();
  if (firstRun) {
    WelcomeDialog welcome;
    welcome.exec();
    position = static_cast<PanelPosition>(welcome.position_->currentIndex());
  } else {
    position = static_cast<PanelPosition>(group.readEntry(
        "position", static_cast<int>(PanelPosition::Bottom)));
  }
  setPosition(position);

  autoHide_ = group.readEntry("autoHide", false);
  autoHideAction_->setChecked(autoHide_);

  showPager_ = group.readEntry("showPager", false);
  pagerAction_->setChecked(showPager_);

  minSize_ = group.readEntry("minimumIconSize", kDefaultMinSize);
  maxSize_ = group.readEntry("maximumIconSize", kDefaultMaxSize);
  if (maxSize_ < minSize_) {
    maxSize_ = minSize_;
  }
  backgroundColor_ = group.readEntry("backgroundColor",
      QColor(kDefaultBackgroundColor));
  backgroundColor_.setAlphaF(kBackgroundAlpha);
  borderColor_ = group.readEntry("borderColor",
      QColor(kDefaultBorderColor));
  tooltipFontSize_ = group.readEntry("tooltipFontSize",
      kDefaultTooltipFontSize);
}

void KSmoothDock::saveConfig() {
  KConfigGroup group(&config_, "General");
  group.writeEntry("position", static_cast<int>(position_));
  group.writeEntry("autoHide", autoHide_);
  group.writeEntry("showPager", showPager_);
  group.writeEntry("minimumIconSize", minSize_);
  group.writeEntry("maximumIconSize", maxSize_);
  group.writeEntry("backgroundColor", backgroundColor_);
  group.writeEntry("borderColor", borderColor_);
  group.writeEntry("tooltipFontSize", tooltipFontSize_);
  config_.sync();
}

void KSmoothDock::initLaunchers() {
  if (!loadLaunchers()) {
    createDefaultLaunchers();
    QDir::root().mkpath(launchersDir_);
    saveLaunchers();
  }
}

bool KSmoothDock::loadLaunchers() {
  if (!QDir::root().exists(launchersDir_)) {
    return false;
  }
  QDir launchersDir(launchersDir_);
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  if (files.isEmpty()) {
    return false;
  }
  items_.reserve(showPager_ ? files.size() + KWindowSystem::numberOfDesktops()
                            : files.size());
  for (int i = 0; i < files.size(); ++i) {
    const QString& file = launchersDir_ + "/" + files.at(i);
    items_.push_back(std::unique_ptr<DockItem>(
        new Launcher(this, file, orientation_, minSize_, maxSize_)));
  }
  return true;
}

void KSmoothDock::createDefaultLaunchers() {
  static const int kNumItems = 7;
  static const char* const kItems[kNumItems][3] = {
    // Name, icon name, command.
    {"Home Folder", "system-file-manager", "dolphin"},
    {"Show Desktop", "user-desktop", kShowDesktopCommand},
    {"Terminal", "utilities-terminal", "konsole"},
    {"Text Editor", "kate", "kate"},
    {"Web Browser", "applications-internet", "firefox"},
    {"Audio Player", "audio-headphones", "amarok"},
    {"System Settings", "preferences-system", "systemsettings5"}
  };
  items_.reserve(showPager_ ? kNumItems + KWindowSystem::numberOfDesktops()
                            : kNumItems);
  for (int i = 0; i < kNumItems; ++i) {
    items_.push_back(std::unique_ptr<DockItem>(
      new Launcher(this, kItems[i][0], orientation_, kItems[i][1], minSize_,
          maxSize_, kItems[i][2])));
  }
}

void KSmoothDock::saveLaunchers() {
  QDir launchersDir(launchersDir_);
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  for (int i = 0; i < files.size(); ++i) {
    launchersDir.remove(files.at(i));
  }

  for (int i = 0; i < numItems(); ++i) {
    Launcher* launcher = dynamic_cast<Launcher*>(items_[i].get());
    if (launcher != nullptr) {
      launcher->saveToFile(QString("%1/%2 - %3.desktop")
          .arg(launchersDir_)
          .arg(i + 1, 2, 10, QChar('0'))
          .arg(items_[i]->label_));
    }
  }
}

void KSmoothDock::initPager() {
  if (showPager_) {
    for (int i = 0; i < KWindowSystem::numberOfDesktops(); ++i) {
      auto* item = new DesktopSelector(
          this, orientation_, minSize_, maxSize_, (i + 1), &config_);
      items_.push_back(std::unique_ptr<DockItem>(item));
      item->init();
    }
  }
}

void KSmoothDock::initLayoutVars() {
  itemSpacing_ = minSize_ / 2;
  parabolicMaxX_ = static_cast<int>(2.5 * (minSize_ + itemSpacing_));
  numAnimationSteps_ = 20;
  animationSpeed_ = 16;

  tooltip_.setFontSize(tooltipFontSize_);
  tooltip_.setFontBold(true);
  tooltip_.setFontItalic(true);
  tooltip_.setFontColor(Qt::white);
  tooltip_.setBackgroundColor(Qt::black);

  const int distance = minSize_ + itemSpacing_;
  // The difference between minWidth_ and maxWidth_
  // (horizontal mode) or between minHeight_ and
  // maxHeight_ (vertical mode).
  int delta = 0;
  if (numItems() >= 5) {
    delta = parabolic(0) + 2 * parabolic(distance) +
        2 * parabolic(2 * distance) - 5 * minSize_;
  } else if (numItems() == 4) {
    delta = parabolic(0) + 2 * parabolic(distance) +
        parabolic(2 * distance) - 4 * minSize_;
  } else if (numItems() == 3) {
    delta = parabolic(0) + 2 * parabolic(distance) - 3 * minSize_;
  } else if (numItems() == 2) {
    delta = parabolic(0) + parabolic(distance) - 2 * minSize_;
  } else if (numItems() == 1) {
    delta = parabolic(0) - minSize_;
  }

  if (orientation_ == Qt::Horizontal) {
    minWidth_ = 0;
    for (const auto& item : items_) {
      minWidth_ += (item->getMinWidth() + itemSpacing_);
    }
    minHeight_ = autoHide_ ? 1 : distance;
    maxWidth_ = minWidth_ + delta;
    maxHeight_ = itemSpacing_ + maxSize_;
  } else {  // Vertical
    minHeight_ = 0;
    for (const auto& item : items_) {
      minHeight_ += (item->getMinHeight() + itemSpacing_);
    }
    minWidth_ = autoHide_ ? 1 : distance;
    maxHeight_ = minHeight_ + delta;
    maxWidth_ = itemSpacing_ + maxSize_;
  }
}

void KSmoothDock::updateLayout() {
  const int distance = minSize_ + itemSpacing_;
  if (isLeaving_) {
    for (const auto& item : items_) {
      item->setAnimationStartAsCurrent();
      if (isHorizontal()) {
        startBackgroundWidth_ = backgroundWidth_;
        startBackgroundHeight_ = distance;
      } else {  // Vertical
        startBackgroundHeight_ = backgroundHeight_;
        startBackgroundWidth_ = distance;
      }
    }
  }

  for (int i = 0; i < numItems(); ++i) {
    items_[i]->size_ = minSize_;
    if (isHorizontal()) {
      items_[i]->left_ = (i == 0) ? itemSpacing_ / 2
          : items_[i - 1]->left_ + items_[i - 1]->getMinWidth() + itemSpacing_;
      items_[i]->top_ = itemSpacing_ / 2;
      items_[i]->minCenter_ = items_[i]->left_ + items_[i]->getMinWidth() / 2;
    } else {  // Vertical
      items_[i]->left_ = itemSpacing_ / 2;
      items_[i]->top_ = (i == 0) ? itemSpacing_ / 2
          : items_[i - 1]->top_ + items_[i - 1]->getMinHeight() + itemSpacing_;
      items_[i]->minCenter_ = items_[i]->top_ + items_[i]->getMinHeight() / 2;
    }
  }
  if (isHorizontal()) {
    backgroundWidth_ = minWidth_;
    backgroundHeight_ = distance;
  } else {  // Vertical
    backgroundHeight_ = minHeight_;
    backgroundWidth_ = distance;
  }

  if (isLeaving_) {
    for (const auto& item : items_) {
      item->endSize_ = item->size_;
      if (isHorizontal()) {
        item->endLeft_ = item->left_ + (maxWidth_ - minWidth_) / 2;
        if (position_ == PanelPosition::Top) {
          item->endTop_ = item->top_ + minHeight_ - distance;
        } else {  // Bottom
          item->endTop_ = item->top_ + (maxHeight_ - minHeight_);
        }
      } else {  // Vertical
        item->endTop_ = item->top_ + (maxHeight_ - minHeight_) / 2;
        if (position_ == PanelPosition::Left) {
          item->endLeft_ = item->left_ + minWidth_ - distance;
        } else {  // Right
          item->endLeft_ = item->left_ + (maxWidth_ - minWidth_);
        }
      }
      item->startAnimation(numAnimationSteps_);
    }
    if (isHorizontal()) {
      endBackgroundWidth_ = minWidth_;
      backgroundWidth_ = startBackgroundWidth_;
      endBackgroundHeight_ = autoHide_ ? 1 : distance;
      backgroundHeight_ = startBackgroundHeight_;
    } else {  // Vertical
      endBackgroundHeight_ = minHeight_;
      backgroundHeight_ = startBackgroundHeight_;
      endBackgroundWidth_ = autoHide_ ? 1 : distance;
      backgroundWidth_ = startBackgroundWidth_;
    }
    currentAnimationStep_ = 0;
    isAnimationActive_ = true;
    animationTimer_->start(32 - animationSpeed_);
  } else {
    resize(minWidth_, minHeight_);
  }
}

void KSmoothDock::updateLayout(int x, int y) {
  const int distance = minSize_ + itemSpacing_;
  if (isEntering_) {
    for (const auto& item : items_) {
      item->startSize_ = item->size_;
      if (isHorizontal()) {
        item->startLeft_ = item->left_ + (maxWidth_ - minWidth_) / 2;
        if (position_ == PanelPosition::Top) {
          item->startTop_ = item->top_ + minHeight_ - distance;
        } else {  // Bottom
          item->startTop_ = item->top_ + (maxHeight_ - minHeight_);
        }
      } else {  // Vertical
        item->startTop_ = item->top_ + (maxHeight_ - minHeight_) / 2;
        if (position_ == PanelPosition::Left) {
          item->startLeft_ = item->left_ + minWidth_ - distance;
        } else {  // Right
          item->startLeft_ = item->left_ + (maxWidth_ - minWidth_);
        }
      }
    }
    if (isHorizontal()) {
      startBackgroundWidth_ = minWidth_;
      startBackgroundHeight_ = autoHide_ ? 1 : distance;
    } else {  // Vertical
      startBackgroundHeight_ = minHeight_;
      startBackgroundWidth_ = autoHide_ ? 1 : distance;
    }
  }

  int first_update_index = -1;
  int last_update_index = 0;
  if (isHorizontal()) {
    items_[0]->left_ = itemSpacing_ / 2;
  } else {  // Vertical
    items_[0]->top_ = itemSpacing_ / 2;
  }
  for (int i = 0; i < numItems(); ++i) {
    int delta;
    if (isHorizontal()) {
      delta = abs(items_[i]->minCenter_ - x + (width() - minWidth_) / 2);
    } else {  // Vertical
      delta = abs(items_[i]->minCenter_ - y + (height() - minHeight_) / 2);
    }
    if (delta < parabolicMaxX_) {
      if (first_update_index == -1) {
        first_update_index = i;
      }
      last_update_index = i;
    }
    items_[i]->size_ = parabolic(delta);
    if (position_ == PanelPosition::Top) {
      items_[i]->top_ = itemSpacing_ / 2;
    } else if (position_ == PanelPosition::Bottom) {
      items_[i]->top_ = itemSpacing_ / 2 + maxSize_ - items_[i]->getHeight();
    } else if (position_ == PanelPosition::Left) {
      items_[i]->left_ = itemSpacing_ / 2;
    } else {  // Right
      items_[i]->left_ = itemSpacing_ / 2 + maxSize_ - items_[i]->getWidth();
    }
    if (i > 0) {
      if (isHorizontal()) {
        items_[i]->left_ = items_[i - 1]->left_ + items_[i - 1]->getWidth()
            + itemSpacing_;
      } else {  // Vertical
        items_[i]->top_ = items_[i - 1]->top_ + items_[i - 1]->getHeight()
            + itemSpacing_;
      }
    }
  }
  for (int i = numItems() - 1; i >= last_update_index + 1; --i) {
    if (isHorizontal()) {
      items_[i]->left_ = (i == numItems() - 1)
          ? maxWidth_ - itemSpacing_ / 2 - items_[i]->getMinWidth()
          : items_[i + 1]->left_ - items_[i]->getMinWidth() - itemSpacing_;
    } else {  // Vertical
      items_[i]->top_ = (i == numItems() - 1)
          ? maxHeight_ - itemSpacing_ / 2 - items_[i]->getMinHeight()
          : items_[i + 1]->top_ - items_[i]->getMinHeight() - itemSpacing_;
    }
  }
  if (first_update_index == 0 && last_update_index < numItems() - 1) {
    for (int i = last_update_index; i >= first_update_index; --i) {
      if (isHorizontal()) {
        items_[i]->left_ = items_[i + 1]->left_ - items_[i]->getWidth()
            - itemSpacing_;
      } else {  // Vertical
        items_[i]->top_ = items_[i + 1]->top_ - items_[i]->getHeight()
            - itemSpacing_;
      }
    }
  }

  if (isEntering_) {
    for (const auto& item : items_) {
      item->setAnimationEndAsCurrent();
      item->startAnimation(numAnimationSteps_);
    }
    if (isHorizontal()) {
      endBackgroundWidth_ = maxWidth_;
      backgroundWidth_ = startBackgroundWidth_;
      endBackgroundHeight_ = distance;
      backgroundHeight_ = startBackgroundHeight_;
    } else {  // Vertical
      endBackgroundHeight_ = maxHeight_;
      backgroundHeight_ = startBackgroundHeight_;
      endBackgroundWidth_ = distance;
      backgroundWidth_ = startBackgroundWidth_;
    }

    currentAnimationStep_ = 0;
    isAnimationActive_ = true;
    isEntering_ = false;
    animationTimer_->start(32 - animationSpeed_);
  }

  resize(maxWidth_, maxHeight_);
  repaint();
}

void KSmoothDock::setStrut() {
  if (position_ == PanelPosition::Top) {
    KWindowSystem::setStrut(winId(), 0, 0, height(), 0);
  } else if (position_ == PanelPosition::Bottom) {
    KWindowSystem::setStrut(winId(), 0, 0, 0, height());
  } else if (position_ == PanelPosition::Left) {
    KWindowSystem::setStrut(winId(), width(), 0, 0, 0);
  } else {  // Right
    KWindowSystem::setStrut(winId(), 0, width(), 0, 0);
  }
}

int KSmoothDock::findActiveItem(int x, int y) {
  int i = 0;
  while (i < numItems() &&
      ((orientation_ == Qt::Horizontal && items_[i]->left_ < x) ||
      (orientation_ == Qt::Vertical && items_[i]->top_ < y))) {
    ++i;
  }
  return i - 1;
}

void KSmoothDock::showTooltip(int i) {
  tooltip_.setText(items_[i]->label_);
  int x, y;
  if (position_ == PanelPosition::Top) {
    x = (desktopWidth_ - width()) / 2 + items_[i]->left_
        - tooltip_.width() / 2 + items_[i]->getWidth() / 2;
    y = maxHeight_ + kTooltipSpacing;
  } else if (position_ == PanelPosition::Bottom) {
    x = (desktopWidth_ - width()) / 2 + items_[i]->left_
        - tooltip_.width() / 2 + items_[i]->getWidth() / 2;
    // No need for additional tooltip spacing in this position.
    y = desktopHeight_ - maxHeight_ - tooltip_.height();
  } else if (position_ == PanelPosition::Left) {
    x = maxWidth_ + kTooltipSpacing;
    y = (desktopHeight_ - height()) / 2 + items_[i]->top_
        - tooltip_.height() / 2 + items_[i]->getHeight() / 2;
  } else {  // Right
    x = desktopWidth_ - maxWidth_ - tooltip_.width() - kTooltipSpacing;
    y = (desktopHeight_ - height()) / 2 + items_[i]->top_
        - tooltip_.height() / 2 + items_[i]->getHeight() / 2;
  }
  tooltip_.move(x, y);
  tooltip_.show();
}

void KSmoothDock::showWaitCursor() {
  setCursor(QCursor(Qt::WaitCursor));
  QTimer::singleShot(1000 /* msecs */, this, SLOT(resetCursor()));
}

int KSmoothDock::parabolic(int x) {
  // Assume x >= 0.
  if (x > parabolicMaxX_) {
    return minSize_;
  } else {
    return maxSize_ -
        (x * x * (maxSize_ - minSize_)) / (parabolicMaxX_ * parabolicMaxX_);
  }
}

}  // namespace ksmoothdock

