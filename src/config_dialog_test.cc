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

#include <cmath>
#include <memory>

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest>

#include <KConfig>
#include <KConfigGroup>

#include "dock_manager.h"
#include "ksmoothdock.h"

namespace ksmoothdock {

class ConfigDialogTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configDir_ = std::make_unique<QTemporaryDir>();
    QVERIFY(configDir_->isValid());
    dockManager_ = std::make_unique<DockManager>(configDir_->path());
    auto& configHelper = dockManager_->configHelper_;
    dockManager_->docks_.push_back(std::make_unique<KSmoothDock>(
        dockManager_.get(),
        configHelper.getDockConfigPath(0),
        configHelper.getDockLaunchersPath(0),
        configHelper.getAppearanceConfigPath()));
    dockManager_->activeDocks_.push_back(dockManager_->docks_[0].get());
    auto* dock = dockManager_->activeDocks_[0];
    dock->init();
    config_ = &dock->appearanceConfig_;

    dialog_ = &dock->configDialog_;
    dialog_->minSize_->setValue(48);
    dialog_->maxSize_->setValue(128);
    dialog_->backgroundAlpha_->setValue(0.42);
    dialog_->backgroundColor_->setColor(QColor("white"));
    dialog_->showBorder_->setChecked(true);
    dialog_->borderColor_->setColor(QColor("white"));
    dialog_->tooltipFontSize_->setValue(20);
    dock->updateConfig();
  }

  // Tests OK button/logic.
  void ok();

  // Tests Apply button/logic.
  void apply();

  // Tests Cancel button/logic.
  void cancel();

 private:
  bool compareDouble(double x, double y) {
    static constexpr double kDelta = 0.01;
    return std::fabs(x - y) < kDelta;
  }

  ConfigDialog* dialog_;
  KConfig* config_;
  std::unique_ptr<DockManager> dockManager_;
  std::unique_ptr<QTemporaryDir> configDir_;
};

void ConfigDialogTest::ok() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundAlpha_->setValue(0.1);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->showBorder_->setChecked(false);
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Ok),
                    Qt::LeftButton);

  // Tests that config values have been updated.
  KConfigGroup group(config_, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 40);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 80);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("green").rgb());
  compareDouble(group.readEntry("backgroundColor", QColor()).alphaF(), 0.1);
  QCOMPARE(group.readEntry("showBorder", true), false);
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("blue"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 24);
}

void ConfigDialogTest::apply() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundAlpha_->setValue(0.1);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->showBorder_->setChecked(false);
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Apply),
                    Qt::LeftButton);

  // Tests that config values have been updated.
  KConfigGroup group(config_, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 40);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 80);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("green").rgb());
  compareDouble(group.readEntry("backgroundColor", QColor()).alphaF(), 0.1);
  QCOMPARE(group.readEntry("showBorder", true), false);
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("blue"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 24);
}

void ConfigDialogTest::cancel() {
  dialog_->minSize_->setValue(40);
  dialog_->maxSize_->setValue(80);
  dialog_->backgroundAlpha_->setValue(0.1);
  dialog_->backgroundColor_->setColor(QColor("green"));
  dialog_->showBorder_->setChecked(false);
  dialog_->borderColor_->setColor(QColor("blue"));
  dialog_->tooltipFontSize_->setValue(24);

  QTest::mouseClick(dialog_->buttonBox_->button(QDialogButtonBox::Cancel),
                    Qt::LeftButton);

  // Tests that config values haven't changed.
  KConfigGroup group(config_, "General");
  QCOMPARE(group.readEntry("minimumIconSize", 0), 48);
  QCOMPARE(group.readEntry("maximumIconSize", 0), 128);
  QCOMPARE(group.readEntry("backgroundColor", QColor()).rgb(),
           QColor("white").rgb());
  compareDouble(group.readEntry("backgroundColor", QColor()).alphaF(), 0.42);
  QCOMPARE(group.readEntry("showBorder", true), true);
  QCOMPARE(group.readEntry("borderColor", QColor()), QColor("white"));
  QCOMPARE(group.readEntry("tooltipFontSize", 0), 20);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::ConfigDialogTest)
#include "config_dialog_test.moc"
