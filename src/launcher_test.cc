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

#include "launcher.h"

#include <QTemporaryFile>
#include <QTest>

namespace ksmoothdock {

class LauncherTest: public QObject {
  Q_OBJECT

 private slots:
  void saveAndLoad();

  void isCommandInternal();
  void isCommandDBus();

  void filterFieldCodes();

 private:
  static const int kMinSize = 64;
  static const int kMaxSize = 64;
};
const int LauncherTest::kMinSize;
const int LauncherTest::kMaxSize;

void LauncherTest::saveAndLoad() {
  Launcher terminal(
      nullptr, "Terminal", Qt::Horizontal, "utilities-terminal", kMinSize,
      kMaxSize, "konsole");
  QTemporaryFile file;
  terminal.saveToFile(file.fileName());

  Launcher terminal2(nullptr, file.fileName(), Qt::Horizontal, kMinSize,
                     kMaxSize);
}

void LauncherTest::isCommandInternal() {
  Launcher terminal(
      nullptr, "Terminal", Qt::Horizontal, "utilities-terminal", kMinSize,
      kMaxSize, "konsole");
  QVERIFY(!terminal.isCommandInternal());

  Launcher show_desktop(
      nullptr, "Show Desktop", Qt::Horizontal, "user-desktop", kMinSize,
      kMaxSize, "SHOW_DESKTOP");
  QVERIFY(show_desktop.isCommandInternal());
}

void LauncherTest::isCommandDBus() {
  Launcher terminal(
      nullptr, "Terminal", Qt::Horizontal, "utilities-terminal", kMinSize,
      kMaxSize, "konsole");
  QVERIFY(!terminal.isCommandDBus());

  Launcher logOut(
      nullptr, "System - Log Out", Qt::Horizontal, "system-log-out", kMinSize,
      kMaxSize, "qdbus org.kde.ksmserver /KSMServer logout 1 0 3");
  QVERIFY(logOut.isCommandDBus());
}

void LauncherTest::filterFieldCodes() {
  QCOMPARE(Launcher::filterFieldCodes("konsole"), QString("konsole"));
  QCOMPARE(Launcher::filterFieldCodes("google-chrome --new-window"),
           QString("google-chrome --new-window"));
  QCOMPARE(Launcher::filterFieldCodes("dolphin %u"), QString("dolphin"));
  QCOMPARE(Launcher::filterFieldCodes("kate -b %U"), QString("kate"));
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::LauncherTest)
#include "launcher_test.moc"
