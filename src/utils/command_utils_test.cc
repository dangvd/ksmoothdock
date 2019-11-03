/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2018 Viet Dang (dangvd@gmail.com)
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

#include "command_utils.h"

#include <QTest>

namespace ksmoothdock {

class CommandUtilsTest: public QObject {
  Q_OBJECT

 private slots:
  void testIsCommandDBus();
  void testFilterFieldCodes();
  void testGetTaskCommand();
  void testAreTheSameCommand();
};

void CommandUtilsTest::testIsCommandDBus() {
  QVERIFY(!isCommandDBus("konsole"));
  QVERIFY(isCommandDBus("qdbus org.kde.ksmserver /KSMServer logout 1 0 3"));
}

void CommandUtilsTest::testFilterFieldCodes() {
  QCOMPARE(filterFieldCodes("konsole"), QString("konsole"));
  QCOMPARE(filterFieldCodes("google-chrome --new-window"),
           QString("google-chrome --new-window"));
  QCOMPARE(filterFieldCodes("dolphin %u"), QString("dolphin"));
  QCOMPARE(filterFieldCodes("kate -b %U"), QString("kate"));
}

void CommandUtilsTest::testGetTaskCommand() {
  QCOMPARE(getTaskCommand(QString("google-chrome")), QString("google-chrome"));
  QCOMPARE(getTaskCommand(QString("/usr/bin/google-chrome")), QString("google-chrome"));
  QCOMPARE(getTaskCommand(QString("/usr/bin/google-chrome http://google.com")),
           QString("google-chrome"));
}

void CommandUtilsTest::testAreTheSameCommand() {
  QVERIFY(areTheSameCommand("google-chrome", "google-chrome"));
  QVERIFY(areTheSameCommand("firefox", "Navigator"));
  QVERIFY(areTheSameCommand("firefox-esr", "Navigator"));
  QVERIFY(areTheSameCommand("thunderbird", "Mail"));
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::CommandUtilsTest)
#include "command_utils_test.moc"
