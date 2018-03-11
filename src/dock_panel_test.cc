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

#include "dock_panel.h"

#include <memory>

#include <QTemporaryDir>
#include <QtTest>

#include <KWindowSystem>

#include "multi_dock_view.h"

namespace ksmoothdock {

constexpr int kDockId = 1;
constexpr int kScreen = 0;
constexpr int kDefaultLauncherCount = 7;

class DockPanelTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    QTemporaryDir configDir;
    model_ = std::make_unique<MultiDockModel>(configDir.path());
    model_->addDock(PanelPosition::Bottom, kScreen);
    view_ = std::make_unique<MultiDockView>(model_.get());
    dock_ = std::make_unique<DockPanel>(view_.get(), model_.get(), kDockId);
  }

  // Tests setting position.
  void setPosition();

  // Tests setting Auto Hide on/off.
  void autoHide();

  // Tests toggling the application menu.
  void toggleApplicationMenu();

  // Tests toggling the pager.
  void togglePager();

  // Tests toggling the clock.
  void toggleClock();

 private:
  void verifyPosition(PanelPosition position) {
    QCOMPARE(dock_->position_, position);
    QCOMPARE(dock_->orientation_,
             (position == PanelPosition::Bottom
                  || position == PanelPosition::Top)
             ? Qt::Horizontal : Qt::Vertical);
    if (dock_->orientation_ == Qt::Horizontal) {
      QVERIFY(dock_->width() > dock_->height());
    } else {
      QVERIFY(dock_->width() < dock_->height());
    }
    QCOMPARE(dock_->positionTop_->isChecked(), position == PanelPosition::Top);
    QCOMPARE(dock_->positionBottom_->isChecked(),
             position == PanelPosition::Bottom);
    QCOMPARE(dock_->positionLeft_->isChecked(),
             position == PanelPosition::Left);
    QCOMPARE(dock_->positionRight_->isChecked(),
             position == PanelPosition::Right);
  }

  void verifyAutoHide(bool enabled) {
    QCOMPARE(dock_->autoHide_, enabled);
    QCOMPARE(dock_->autoHideAction_->isChecked(), enabled);
    if (enabled) {
      QVERIFY(dock_->width() == 1 || dock_->height() == 1);
    }
  }

  void verifyApplicationMenu(bool enabled) {
    QCOMPARE(dock_->showApplicationMenu_, enabled);
    QCOMPARE(dock_->applicationMenuAction_->isChecked(), enabled);
    if (enabled) {
      QCOMPARE(dock_->itemCount(), kDefaultLauncherCount + 1);
    } else {
      QCOMPARE(dock_->itemCount(), kDefaultLauncherCount);
    }
  }

  void verifyPager(bool enabled) {
    QCOMPARE(dock_->showPager_, enabled);
    QCOMPARE(dock_->pagerAction_->isChecked(), enabled);
    if (enabled) {
      QCOMPARE(dock_->itemCount(),
               kDefaultLauncherCount + KWindowSystem::numberOfDesktops() + 1);
    } else {
      QCOMPARE(dock_->itemCount(), kDefaultLauncherCount + 1);
    }
  }

  void verifyClock(bool enabled) {
    QCOMPARE(dock_->showClock_, enabled);
    QCOMPARE(dock_->clockAction_->isChecked(), enabled);
    if (enabled) {
      QCOMPARE(dock_->itemCount(), kDefaultLauncherCount + 2);
    } else {
      QCOMPARE(dock_->itemCount(), kDefaultLauncherCount + 1);
    }
  }

  std::unique_ptr<MultiDockModel> model_;
  std::unique_ptr<MultiDockView> view_;
  std::unique_ptr<DockPanel> dock_;
};

void DockPanelTest::setPosition() {
  verifyPosition(PanelPosition::Bottom);
  dock_->positionLeft_->trigger();
  verifyPosition(PanelPosition::Left);
  dock_->positionRight_->trigger();
  verifyPosition(PanelPosition::Right);
  dock_->positionTop_->trigger();
  verifyPosition(PanelPosition::Top);
  dock_->positionBottom_->trigger();
  verifyPosition(PanelPosition::Bottom);
}

void DockPanelTest::autoHide() {
  verifyAutoHide(false);
  dock_->autoHideAction_->trigger();
  verifyAutoHide(true);
  dock_->autoHideAction_->trigger();
  verifyAutoHide(false);
}

void DockPanelTest::toggleApplicationMenu() {
  verifyApplicationMenu(true);
  dock_->applicationMenuAction_->trigger();
  verifyApplicationMenu(false);
  dock_->applicationMenuAction_->trigger();
  verifyApplicationMenu(true);
}

void DockPanelTest::togglePager() {
  verifyPager(false);
  // TODO: Test when the pager is enabled.
}

void DockPanelTest::toggleClock() {
  verifyClock(false);
  dock_->clockAction_->trigger();
  verifyClock(true);
  dock_->clockAction_->trigger();
  verifyClock(false);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::DockPanelTest)
#include "dock_panel_test.moc"
