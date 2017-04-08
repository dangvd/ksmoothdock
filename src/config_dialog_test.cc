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

#include "config_dialog.h"

#include <memory>

#include <QTemporaryFile>
#include <QtTest>

#include <KConfig>
#include <KConfigGroup>

#include "ksmoothdock.h"

namespace ksmoothdock {

class ConfigDialogTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configFile_.reset(new QTemporaryFile);
    QVERIFY(configFile_->open());

    dock_.reset(new KSmoothDock(configFile_->fileName(),
                                "" /* launchersPath */));
    dialog_ = dock_->configDialog();
    dialog_->minSize_->setValue(48);
    dialog_->maxSize_->setValue(128);
    dock_->updateConfig();

    dialog_->show();
  }

  // Tests Cancel button/logic.
  void cancel();

 private:
  ConfigDialog* dialog_;
  std::unique_ptr<KSmoothDock> dock_;
  std::unique_ptr<QTemporaryFile> configFile_;
};

void ConfigDialogTest::cancel() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Cancel),
                    Qt::LeftButton);

  QVERIFY(!dialog_->isVisible());
  KConfig config(configFile_->fileName(), KConfig::SimpleConfig);
  KConfigGroup group(&config, "General");
  // Tests that config values haven't changed.
  QCOMPARE(group.readEntry("minimumIconSize", 0), 48);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 128);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::ConfigDialogTest)
#include "config_dialog_test.moc"
