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

#include <QTemporaryDir>
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
    launchersDir_.reset(new QTemporaryDir);
    QVERIFY(launchersDir_->isValid());

    dock_.reset(new KSmoothDock(configFile_->fileName(),
                                launchersDir_->path()));
    dialog_ = dock_->configDialog();
    dialog_->minSize_->setValue(48);
    dialog_->maxSize_->setValue(128);
    dialog_->backgroundColor_->setColor(QColor("white"));
    dialog_->borderColor_->setColor(QColor("white"));
    dialog_->tooltipFontSize_->setValue(20);
    dock_->updateConfig();
  }

  // Tests OK button/logic.
  void ok();

  // Tests Apply button/logic.
  void apply();

  // Tests Cancel button/logic.
  void cancel();

 private:
  ConfigDialog* dialog_;
  std::unique_ptr<KSmoothDock> dock_;
  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<QTemporaryDir> launchersDir_;
};

void ConfigDialogTest::ok() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Ok),
                    Qt::LeftButton);

  // Tests that config values have been updated.
  KConfig config(configFile_->fileName(), KConfig::SimpleConfig);
  KConfigGroup group(&config, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 40);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 80);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("green").rgb());
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("blue"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 24);
}

void ConfigDialogTest::apply() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Apply),
                    Qt::LeftButton);

  // Tests that config values have been updated.
  KConfig config(configFile_->fileName(), KConfig::SimpleConfig);
  KConfigGroup group(&config, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 40);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 80);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("green").rgb());
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("blue"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 24);
}

void ConfigDialogTest::cancel() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Cancel),
                    Qt::LeftButton);

  // Tests that config values haven't changed.
  KConfig config(configFile_->fileName(), KConfig::SimpleConfig);
  KConfigGroup group(&config, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 48);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 128);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("white").rgb());
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("white"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 20);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::ConfigDialogTest)
#include "config_dialog_test.moc"
