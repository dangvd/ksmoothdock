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

#include "application_menu_settings_dialog.h"
#include "ui_application_menu_settings_dialog.h"

namespace ksmoothdock {

ApplicationMenuSettingsDialog::ApplicationMenuSettingsDialog(
    MultiDockModel* model)
    : QDialog(nullptr),
      ui(new Ui::ApplicationMenuSettingsDialog),
      model_(model) {
  ui->setupUi(this);
  icon_ = new KIconButton(this);
  icon_->setGeometry(QRect(140, 80, 80, 80));

  connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
      this, SLOT(buttonClicked(QAbstractButton*)));

  loadData();
}

ApplicationMenuSettingsDialog::~ApplicationMenuSettingsDialog() {
  delete ui;
}

void ApplicationMenuSettingsDialog::accept() {
  QDialog::accept();
  saveData();
}

void ApplicationMenuSettingsDialog::buttonClicked(QAbstractButton *button) {
  auto role = ui->buttonBox->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    saveData();
  }
}

void ApplicationMenuSettingsDialog::loadData() {
  ui->name->setText(model_->applicationMenuName());
  icon_->setIcon(model_->applicationMenuIcon());
}

void ApplicationMenuSettingsDialog::saveData() {
  model_->setApplicationMenuName(ui->name->text());
  model_->setApplicationMenuIcon(icon_->icon());
  model_->saveAppearanceConfig();
}

}  // namespace ksmoothdock
