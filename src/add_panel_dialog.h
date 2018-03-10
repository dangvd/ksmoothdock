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

#ifndef KSMOOTHDOCK_ADD_PANEL_DIALOG_H_
#define KSMOOTHDOCK_ADD_PANEL_DIALOG_H_

#include <QDialog>

#include "multi_dock_model.h"

namespace Ui {
  class AddPanelDialog;
}

namespace ksmoothdock {

class AddPanelDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AddPanelDialog(MultiDockModel* model);
  ~AddPanelDialog();

 public slots:
  void accept() override;

 private:
  Ui::AddPanelDialog *ui;

  MultiDockModel* model_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_ADD_PANEL_DIALOG_H_
