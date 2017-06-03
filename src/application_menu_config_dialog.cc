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

#include "application_menu_config_dialog.h"

#include <KLocalizedString>

#include "application_menu.h"
#include "ksmoothdock.h"

namespace ksmoothdock {

ApplicationMenuConfigDialog::ApplicationMenuConfigDialog(
    KSmoothDock* parent, ApplicationMenu* applicationMenu)
    : QDialog(parent), applicationMenu_(applicationMenu) {
  setWindowTitle(i18n("Application Menu Settings"));
  resize(500, 270);

  nameLabel_ = new QLabel(this);
  nameLabel_->setText(i18n("Name"));
  nameLabel_->setGeometry(QRect(40, 30, 70, 22));
  name_ = new QLineEdit(this);
  name_->setGeometry(QRect(140, 20, 320, 36));

  iconLabel_ = new QLabel(this);
  iconLabel_->setText(i18n("Icon"));
  iconLabel_->setGeometry(QRect(40, 110, 72, 22));
  icon_ = new KIconButton(this);
  icon_->setGeometry(QRect(140, 80, 80, 80));

  buttonBox_ = new QDialogButtonBox(this);
  buttonBox_->setGeometry(QRect(40, 200, 420, 32));
  buttonBox_->setOrientation(Qt::Horizontal);
  buttonBox_->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Ok
      |QDialogButtonBox::Cancel);
  buttonBox_->setCenterButtons(true);
  connect(buttonBox_, SIGNAL(accepted()),
          applicationMenu_, SLOT(updateConfig()));
  connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
  connect(buttonBox_, SIGNAL(clicked(QAbstractButton*)), this,
      SLOT(buttonClicked(QAbstractButton*)));
}

void ApplicationMenuConfigDialog::buttonClicked(QAbstractButton* button) {
  auto role = buttonBox_->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    applicationMenu_->applyConfig();
  }
}

}  // namespace ksmoothdock
