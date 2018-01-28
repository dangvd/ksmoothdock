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

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>

namespace ksmoothdock {

class AddPanelDialog : public QDialog {
  Q_OBJECT

 public:
  AddPanelDialog();
  ~AddPanelDialog() = default;

 private:
  QLabel *positionLabel_;
  QComboBox *position_;
  QLabel *screenLabel_;
  QComboBox *screen_;
  QDialogButtonBox *buttonBox_;

  friend class KSmoothDock;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_ADD_PANEL_DIALOG_H_
