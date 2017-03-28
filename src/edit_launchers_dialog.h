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
#include <QDataStream>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMetaType>

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

QDataStream &operator<<(QDataStream &out, const LauncherInfo& launcher);
QDataStream &operator>>(QDataStream &in, LauncherInfo& launcher);

class EditLaunchersDialog;

class LauncherList : public QListWidget {
 public:
  explicit LauncherList(EditLaunchersDialog* parent);

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dropEvent(QDropEvent *event) override;

 private:
  EditLaunchersDialog* parent_;
};

class EditLaunchersDialog : public QDialog {
  Q_OBJECT

 public:
  EditLaunchersDialog(KSmoothDock* parent);
  ~EditLaunchersDialog() {}

  void addLauncher(const QString& name, const QString& command,
      const QString& iconName);

 public slots:
  void buttonClicked(QAbstractButton* button);

  void refreshSelectedLauncher(QListWidgetItem* current,
      QListWidgetItem* previous);

  void addLauncher();
  void removeSelectedLauncher();
  void updateSelectedLauncher();

  void browseCommand();
  void updateInternalCommand(int index);
  void updateDBusCommand(int index);

 private:
  QIcon getListItemIcon(const QString& iconName) {
    return QIcon(KIconLoader::global()->loadIcon(iconName,
        KIconLoader::NoGroup, kListIconSize));
  }

  void populateInternalCommands();

  void populateDBusCommands();

  KSmoothDock* parent_;

  LauncherList *launchers_;

  QPushButton *add_;
  QPushButton *remove_;
  QPushButton *update_;

  QLabel *nameLabel_;
  QLineEdit *name_;
  QLabel *commandLabel_;
  QLineEdit *command_;
  QPushButton *browseCommand_;
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
