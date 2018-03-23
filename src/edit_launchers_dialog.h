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
#include <QDataStream>
#include <QDialog>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMetaType>

#include <KIconButton>
#include <KIconLoader>

#include "multi_dock_model.h"

namespace Ui {
  class EditLaunchersDialog;
}

namespace ksmoothdock {

static constexpr int kListIconSize = 48;

// User data for the items in QListWidget/QComboBox.
struct LauncherInfo {
  // The name(label) is already stored as item text in QListWidget/QComboBox.
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
  EditLaunchersDialog(QWidget* parent, MultiDockModel* model, int dockId);
  ~EditLaunchersDialog() = default;

  void reload() { loadData(); }

  void addLauncher(const QString& name, const QString& command,
      const QString& iconName);

 public slots:
  void accept() override;
  void buttonClicked(QAbstractButton* button);

  void refreshSelectedLauncher(QListWidgetItem* current,
      QListWidgetItem* previous);

  void addLauncher();
  void removeSelectedLauncher();
  void updateSelectedLauncher();

  void openLink(const QString& link);

  void browseCommand();
  void updateInternalCommand(int index);
  void updateDBusCommand(int index);
  void updateWebCommand(int index);
  void resetCommandLists();

 private:
  void loadData();
  void saveData();

  QIcon getListItemIcon(const QString& iconName) {
    return QIcon(KIconLoader::global()->loadIcon(iconName,
        KIconLoader::NoGroup, kListIconSize));
  }

  void populateInternalCommands();
  void populateDBusCommands();
  void populateWebCommands();

  Ui::EditLaunchersDialog *ui;
  LauncherList *launchers_;
  KIconButton *icon_;

  MultiDockModel* model_;
  int dockId_;

  friend class EditLaunchersDialogTest;
};

}  // namespace ksmoothdock

Q_DECLARE_METATYPE(ksmoothdock::LauncherInfo);

#endif  // KSMOOTHDOCK_EDIT_LAUNCHERS_DIALOG_H_
