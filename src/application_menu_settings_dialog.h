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

#ifndef KSMOOTHDOCK_APPLICATION_MENU_SETTINGS_DIALOG_H_
#define KSMOOTHDOCK_APPLICATION_MENU_SETTINGS_DIALOG_H_

#include <QDialog>

#include <KIconButton>

#include "multi_dock_model.h"

namespace Ui {
  class ApplicationMenuSettingsDialog;
}

namespace ksmoothdock {

class ApplicationMenuSettingsDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ApplicationMenuSettingsDialog(MultiDockModel* model);
  ~ApplicationMenuSettingsDialog();

  void reload() { loadData(); }

 public slots:
  void accept() override;
  void buttonClicked(QAbstractButton* button);

 private:
  void loadData();
  void saveData();

  Ui::ApplicationMenuSettingsDialog *ui;
  KIconButton* icon_;

  MultiDockModel* model_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_APPLICATION_MENU_SETTINGS_DIALOG_H_
