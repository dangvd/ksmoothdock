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

#ifndef KSMOOTHDOCK_EDIT_LAUNCHERS_DIALOG_H_
#define KSMOOTHDOCK_EDIT_LAUNCHERS_DIALOG_H_

#include <QAbstractButton>
#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>

#include <KIconButton>
#include <KIconLoader>

namespace ksmoothdock {

static constexpr int kListIconSize = 48;

class KSmoothDock;

// User data for the items in the QListWidget.
struct LauncherInfo {
  // The name(label) is already stored as item text in the QListWidget.
  QString iconName;
  QString command;

  LauncherInfo() {}
  LauncherInfo(QString iconName2, QString command2)
      : iconName(iconName2), command(command2) {}
};

class EditLaunchersDialog : public QDialog {
  Q_OBJECT

 public:
  EditLaunchersDialog(KSmoothDock* parent);
  ~EditLaunchersDialog() {}

 public slots:
  void buttonClicked(QAbstractButton* button);

  void updateInternalCommand(int index);
  void updateDBusCommand(int index);

  void refreshSelectedLauncher(QListWidgetItem* current,
      QListWidgetItem* previous);

  void addLauncher();
  void removeSelectedLauncher();
  void updateSelectedLauncher();

 private:
  QIcon getListItemIcon(const QString& iconName) {
    return QIcon(KIconLoader::global()->loadIcon(iconName,
        KIconLoader::NoGroup, kListIconSize));
  }

  void populateInternalCommands();

  void populateDBusCommands();

  KSmoothDock* parent_;

  QListWidget *launchers_;

  QPushButton *add_;
  QPushButton *remove_;
  QPushButton *update_;
  QPushButton *openDir_;
  QPushButton *syncFromDir_;

  QLabel *nameLabel_;
  QLineEdit *name_;
  QLabel *commandLabel_;
  QLineEdit *command_;
  QPushButton *browseExecutable_;
  QComboBox *internalCommands_;
  QComboBox *dbusCommands_;
  QLabel *iconLabel_;
  KIconButton *icon_;

  QDialogButtonBox *buttonBox_;

  friend class KSmoothDock;
};

}  // namespace ksmoothdock

Q_DECLARE_METATYPE(ksmoothdock::LauncherInfo);

#endif  // KSMOOTHDOCK_EDIT_LAUNCHERS_DIALOG_H_
