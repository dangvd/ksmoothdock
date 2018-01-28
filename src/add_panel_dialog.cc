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

#include "add_panel_dialog.h"

#include <QApplication>
#include <QDesktopWidget>

#include <KLocalizedString>

namespace ksmoothdock {

AddPanelDialog::AddPanelDialog() {
  setWindowTitle(i18n("Add a new panel"));
  resize(400, 200);

  positionLabel_ = new QLabel(this);
  positionLabel_->setText(i18n("Panel position"));
  positionLabel_->setGeometry(QRect(30, 30, 211, 22));
  position_ = new QComboBox(this);
  position_->addItem(i18n("Top"));
  position_->addItem(i18n("Bottom"));
  position_->addItem(i18n("Left"));
  position_->addItem(i18n("Right"));
  position_->setCurrentIndex(1);
  position_->setGeometry(QRect(260, 20, 111, 36));

  const int numScreens = QApplication::desktop()->screenCount();
  if (numScreens > 1) {
    screenLabel_ = new QLabel(this);
    screenLabel_->setText(i18n("Screen"));
    screenLabel_->setGeometry(QRect(30, 80, 211, 22));
    screen_ = new QComboBox(this);
    for (int i = 1; i <= numScreens; ++i) {
      screen_->addItem(QString::number(i));
    }
    screen_->setCurrentIndex(0);
    screen_->setGeometry(QRect(260, 70, 111, 36));
  }

  buttonBox_ = new QDialogButtonBox(this);
  buttonBox_->setGeometry(QRect(30, 140, 340, 32));
  buttonBox_->setOrientation(Qt::Horizontal);
  buttonBox_->setStandardButtons(QDialogButtonBox::Ok |
                                 QDialogButtonBox::Cancel);
  buttonBox_->setCenterButtons(true);

  connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
}

}  // namespace ksmoothdock
