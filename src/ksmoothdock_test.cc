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

  // Test creating default launchers.
  void createDefaultLaunchers();

 private:
  std::unique_ptr<KSmoothDock> dock_;
  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<QTemporaryDir> launchersDir_;
};

void KSmoothDockTest::createDefaultLaunchers() {
  // Tests that default launchers have been created.
  QDir launchersDir(launchersDir_->path());
  QStringList files = launchersDir.entryList(QDir::Files, QDir::Name);
  QCOMPARE(files.size(), 7);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::KSmoothDockTest)
#include "ksmoothdock_test.moc"
