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

#include "edit_launchers_dialog.h"
#include "ui_edit_launchers_dialog.h"

#include <memory>

#include <QDir>
#include <QStringList>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest>

#include <KConfig>
#include <KConfigGroup>

namespace ksmoothdock {

constexpr int kDockId = 0;

class EditLaunchersDialogTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    dialog_ = std::make_unique<EditLaunchersDialog>(&model_, kDockId);
  }

  // Tests OK button/logic.
  void ok();

  // Tests Apply button/logic.
  void apply();

  // Tests Cancel button/logic.
  void cancel();

 private:
  int launcherCount() {
    return static_cast<int>(model_.launcherConfigs(kDockId).size());
  }

  MultiDockModel model_;
  std::unique_ptr<EditLaunchersDialog> dialog_;
};

void EditLaunchersDialogTest::ok() {
  dialog_->addLauncher("Text Editor", "kate", "kate");
  QTest::mouseClick(dialog_->ui->buttonBox->button(QDialogButtonBox::Ok),
                    Qt::LeftButton);

  // Verify.
  QCOMPARE(launcherCount(), 3);
}

void EditLaunchersDialogTest::apply() {
  dialog_->addLauncher("Text Editor", "kate", "kate");
  QTest::mouseClick(dialog_->ui->buttonBox->button(QDialogButtonBox::Apply),
                    Qt::LeftButton);

  // Verify.
  QCOMPARE(launcherCount(), 3);
}

void EditLaunchersDialogTest::cancel() {
  dialog_->addLauncher("Text Editor", "kate", "kate");
  QTest::mouseClick(dialog_->ui->buttonBox->button(QDialogButtonBox::Cancel),
                    Qt::LeftButton);

  // Verify.
  QCOMPARE(launcherCount(), 2);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::EditLaunchersDialogTest)
#include "edit_launchers_dialog_test.moc"
