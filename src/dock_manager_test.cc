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

#include "dock_manager.h"

#include <memory>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

namespace ksmoothdock {

class DockManagerTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configDir_ = std::make_unique<QTemporaryDir>();
    QVERIFY(configDir_->isValid());
    dockManager_ = std::make_unique<DockManager>(configDir_->path());
  }

  void loadDocks_empty() {
    QVERIFY(!dockManager_->loadDocks());
    QVERIFY(dockManager_->docks_.empty());
    QVERIFY(dockManager_->activeDocks_.empty());
  }

  void loadDocks_singleDock() {
    auto& configHelper = dockManager_->configHelper_;
    createFile(configHelper.getDockConfigPath(0));
    createDir(configHelper.getDockLaunchersPath(0));
    QVERIFY(dockManager_->loadDocks());
    QVERIFY(dockManager_->docks_.size() == 1);
  }

  void loadDocks_multipleDocks() {
    auto& configHelper = dockManager_->configHelper_;
    createFile(configHelper.getDockConfigPath(0));
    createDir(configHelper.getDockLaunchersPath(0));
    createFile(configHelper.getDockConfigPath(1));
    createDir(configHelper.getDockLaunchersPath(1));
    QVERIFY(dockManager_->loadDocks());
    QVERIFY(dockManager_->docks_.size() == 2);
  }

 private:
  void createFile(const QString& filePath) {
    QFile(filePath).open(QIODevice::WriteOnly);
  }

  void createDir(const QString& dirPath) {
    QDir::root().mkpath(dirPath);
  }

  std::unique_ptr<DockManager> dockManager_;
  std::unique_ptr<QTemporaryDir> configDir_;
};

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::DockManagerTest)
#include "dock_manager_test.moc"
