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

#include "application_menu.h"

#include <memory>

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>

namespace ksmoothdock {

class ApplicationMenuTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configFile_.reset(new QTemporaryFile);
    QVERIFY(configFile_->open());
    config_.reset(new KConfig(configFile_->fileName(), KConfig::SimpleConfig));
  }

  void loadEntries_singleDir();

 private:
  static constexpr int kMinSize = 64;
  static constexpr int kMaxSize = 64;
  static const int kNumCategories = 11;

  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<KConfig> config_;
};
const int ApplicationMenuTest::kNumCategories;

void ApplicationMenuTest::loadEntries_singleDir() {
  QTemporaryDir entryDir;
  QVERIFY(entryDir.isValid());

  ApplicationMenu applicationMenu(nullptr, Qt::Horizontal, kMinSize, kMaxSize,
                                  config_.get(), { entryDir.path() });
  applicationMenu.initCategories();
  applicationMenu.loadEntries();

  QCOMPARE(static_cast<int>(applicationMenu.categories_.size()),
           kNumCategories);
  for (const auto& category : applicationMenu.categories_) {
    QVERIFY(category.entries.empty());
  }
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::ApplicationMenuTest)
#include "application_menu_test.moc"
