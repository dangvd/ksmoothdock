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

#include "welcome_dialog.h"
#include "ui_welcome_dialog.h"

#include <memory>

#include <QPushButton>
#include <QTemporaryDir>
#include <QtTest>

namespace ksmoothdock {

class WelcomeDialogTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    QTemporaryDir configDir;
    model_ = std::make_unique<MultiDockModel>(configDir.path());
    dialog_ = std::make_unique<WelcomeDialog>(model_.get());
  }

  // Tests OK button.
  void ok();

 private:
  std::unique_ptr<MultiDockModel> model_;
  std::unique_ptr<WelcomeDialog> dialog_;
};

void WelcomeDialogTest::ok() {
  QTest::mouseClick(dialog_->ui->buttonBox->button(QDialogButtonBox::Ok),
                    Qt::LeftButton);
  QCOMPARE(model_->dockCount(), 1);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::WelcomeDialogTest)
#include "welcome_dialog_test.moc"
