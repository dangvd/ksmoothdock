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

#include <memory>

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest>

#include <KWindowSystem>

namespace ksmoothdock {

class KSmoothDockTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configFile_.reset(new QTemporaryFile);
    QVERIFY(configFile_->open());
    launchersDir_.reset(new QTemporaryDir);
    QVERIFY(launchersDir_->isValid());
    dock_.reset(new KSmoothDock(configFile_->fileName(),
                                launchersDir_->path()));
    dock_->init();
  }

  // Tests creating default launchers.
  void createDefaultLaunchers();

  // Tests setting position.
  void setPosition();

  // Tests setting Auto Hide on/off.
  void autoHide();

  // Tests toggling the pager.
  void togglePager();

  // Tests toggling the clock.
  void toggleClock();

 private:
  static const int kNumDefaultLaunchers = 6;

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

  void verifyPager(bool enabled) {
    QCOMPARE(dock_->showPager_, enabled);
    QCOMPARE(dock_->pagerAction_->isChecked(), enabled);
    if (enabled) {
      QCOMPARE(dock_->numItems(),
               kNumDefaultLaunchers + KWindowSystem::numberOfDesktops() + 2);
    } else {
      QCOMPARE(dock_->numItems(), kNumDefaultLaunchers + 2);
    }
  }

  void verifyClock(bool enabled) {
    QCOMPARE(dock_->showClock_, enabled);
    QCOMPARE(dock_->clockAction_->isChecked(), enabled);
    if (enabled) {
      QCOMPARE(dock_->numItems(),
               kNumDefaultLaunchers + KWindowSystem::numberOfDesktops() + 2);
    } else {
      QCOMPARE(dock_->numItems(),
               kNumDefaultLaunchers + KWindowSystem::numberOfDesktops() + 1);
    }
  }

  std::unique_ptr<KSmoothDock> dock_;
  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<QTemporaryDir> launchersDir_;
};
const int KSmoothDockTest::kNumDefaultLaunchers;

void KSmoothDockTest::createDefaultLaunchers() {
  // Tests that default launchers have been created.
  QDir launchersDir(launchersDir_->path());
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  QCOMPARE(files.size(), kNumDefaultLaunchers);
}

void KSmoothDockTest::setPosition() {
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

void KSmoothDockTest::autoHide() {
  verifyAutoHide(false);
  dock_->autoHideAction_->trigger();
  verifyAutoHide(true);
  dock_->autoHideAction_->trigger();
  verifyAutoHide(false);
}

void KSmoothDockTest::togglePager() {
  verifyPager(true);
  // TODO: Test when the pager is enabled.
}

void KSmoothDockTest::toggleClock() {
  verifyClock(true);
  dock_->clockAction_->trigger();
  verifyClock(false);
  dock_->clockAction_->trigger();
  verifyClock(true);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::KSmoothDockTest)
#include "ksmoothdock_test.moc"
