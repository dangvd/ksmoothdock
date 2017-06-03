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

#ifndef KSMOOTHDOCK_APPLICATION_MENU_CONFIG_DIALOG_H_
#define KSMOOTHDOCK_APPLICATION_MENU_CONFIG_DIALOG_H_

#include <QAbstractButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>

#include <KIconButton>

namespace ksmoothdock {

class ApplicationMenu;
class KSmoothDock;

class ApplicationMenuConfigDialog : public QDialog {
  Q_OBJECT

 public:
  ApplicationMenuConfigDialog(KSmoothDock* parent,
                              ApplicationMenu* applicationMenu);

 public slots:
  void buttonClicked(QAbstractButton* button);

 private:
  ApplicationMenu* applicationMenu_;

  QLabel *nameLabel_;
  QLineEdit *name_;
  QLabel *iconLabel_;
  KIconButton *icon_;

  QDialogButtonBox *buttonBox_;

  friend class ApplicationMenu;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_APPLICATION_MENU_CONFIG_DIALOG_H_
