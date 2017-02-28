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

#include "welcome_dialog.h"

#include <KLocalizedString>

namespace ksmoothdock {

WelcomeDialog::WelcomeDialog() {
  setWindowTitle(i18n("Welcome to KSmoothDock!"));
  resize(480, 150);

  positionLabel_ = new QLabel(this);
  positionLabel_->setText(i18n("Please select the initial panel position"));
  positionLabel_->setGeometry(QRect(30, 30, 291, 16));
  position_ = new QComboBox(this);
  position_->addItem(i18n("Top"));
  position_->addItem(i18n("Bottom"));
  position_->addItem(i18n("Left"));
  position_->addItem(i18n("Right"));
  position_->setCurrentIndex(1);
  position_->setGeometry(QRect(340, 20, 111, 36));

  buttonBox_ = new QDialogButtonBox(this);
  buttonBox_->setGeometry(QRect(70, 90, 341, 32));
  buttonBox_->setOrientation(Qt::Horizontal);
  buttonBox_->setStandardButtons(QDialogButtonBox::Ok);
  buttonBox_->setCenterButtons(true);

  connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
}

}  // namespace ksmoothdock
