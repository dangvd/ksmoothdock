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

#include "edit_launchers_dialog.h"

#include <QFileDialog>
#include <QMimeData>
#include <QUrl>
#include <QVariant>

#include <KDesktopFile>
#include <KLocalizedString>

#include "ksmoothdock.h"
#include "launcher.h"

namespace ksmoothdock {

QDataStream &operator<<(QDataStream &out, const LauncherInfo& launcher) {
  out << launcher.iconName << launcher.command;
  return out;
}

QDataStream &operator>>(QDataStream &in, LauncherInfo& launcher) {
  in >> launcher.iconName >> launcher.command;
  return in;
}

LauncherList::LauncherList(EditLaunchersDialog* parent)
    : QListWidget(parent), parent_(parent) {}

void LauncherList::dragEnterEvent(QDragEnterEvent *event) {
  // Internal drag-and-drop.
  LauncherList* source = dynamic_cast<LauncherList*>(event->source());
  if (source != nullptr && source == this) {
    event->acceptProposedAction();
    setDragDropMode(QAbstractItemView::InternalMove);
    return;
  }

  // External drag-and-drop.
  if (event->mimeData()->hasFormat("text/uri-list")) {
    QString fileUrl =
        QString(event->mimeData()->data("text/uri-list")).trimmed();
    if (fileUrl.endsWith(".desktop")) {
      event->acceptProposedAction();
      setDragDropMode(QAbstractItemView::DragDrop);
    }
  }
}

void LauncherList::dragMoveEvent(QDragMoveEvent* event) {
  event->acceptProposedAction();
}

void LauncherList::dropEvent(QDropEvent* event) {
  // External drag-and-drop.
  if (event->mimeData()->hasFormat("text/uri-list")) {
    QString fileUrl =
        QString(event->mimeData()->data("text/uri-list")).trimmed();
    KDesktopFile desktopFile(QUrl(fileUrl).toLocalFile());
    const QString name = desktopFile.readName();
    const QString command = Launcher::filterFieldCodes(
        desktopFile.entryMap("Desktop Entry")["Exec"]);
    const QString iconName = desktopFile.readIcon();
    parent_->addLauncher(name, command, iconName);
  } else {  // Internal drag-and-drop.
    QListWidget::dropEvent(event);
  }
}

EditLaunchersDialog::EditLaunchersDialog(KSmoothDock* parent)
    : QDialog(parent), parent_(parent) {
  setWindowTitle(i18n("Edit Launchers"));
  resize(1120, 610);

  qRegisterMetaType<LauncherInfo>();
  qRegisterMetaTypeStreamOperators<LauncherInfo>("LauncherInfo");

  launchers_ = new LauncherList(this);
  launchers_->setGeometry(QRect(20, 20, 441, 491));
  connect(
      launchers_,
      SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
      this,
      SLOT(refreshSelectedLauncher(QListWidgetItem*, QListWidgetItem*)));
  launchers_->setSelectionMode(QAbstractItemView::SingleSelection);
  launchers_->setDragEnabled(true);
  launchers_->setAcceptDrops(true);
  launchers_->setDropIndicatorShown(true);
  launchers_->setDragDropMode(QAbstractItemView::DragDrop);

  add_ = new QPushButton(this);
  add_->setText(i18n("Add"));
  add_->setIcon(QIcon::fromTheme("list-add"));
  add_->setGeometry(QRect(500, 120, 121, 38));
  connect(add_, SIGNAL(clicked()), this, SLOT(addLauncher()));

  remove_ = new QPushButton(this);
  remove_->setText(i18n("Remove"));
  remove_->setIcon(QIcon::fromTheme("list-remove"));
  remove_->setGeometry(QRect(500, 190, 121, 38));
  connect(remove_, SIGNAL(clicked()), this, SLOT(removeSelectedLauncher()));

  update_ = new QPushButton(this);
  update_->setText(i18n("Update"));
  update_->setIcon(QIcon::fromTheme("arrow-left"));
  update_->setGeometry(QRect(500, 260, 121, 38));
  connect(update_, SIGNAL(clicked()), this, SLOT(updateSelectedLauncher()));

  nameLabel_ = new QLabel(this);
  nameLabel_->setText(i18n("Name"));
  nameLabel_->setGeometry(QRect(660, 30, 72, 22));
  name_ = new QLineEdit(this);
  name_->setGeometry(QRect(770, 20, 331, 36));

  commandLabel_ = new QLabel(this);
  commandLabel_->setText(i18n("Command"));
  commandLabel_->setGeometry(QRect(660, 110, 101, 22));
  command_ = new QLineEdit(this);
  command_->setGeometry(QRect(770, 100, 331, 36));
  connect(command_, SIGNAL(textEdited(const QString&)),
      this, SLOT(resetInternalAndDBusCommands()));

  browseCommand_ = new QPushButton(this);
  browseCommand_->setText(i18n("Browse Command"));
  connect(browseCommand_, SIGNAL(clicked()),
      this, SLOT(browseCommand()));
  browseCommand_->setGeometry(QRect(770, 160, 251, 38));

  internalCommands_ = new QComboBox(this);
  populateInternalCommands();
  internalCommands_->setGeometry(QRect(770, 220, 251, 36));
  connect(internalCommands_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(updateInternalCommand(int)));

  dbusCommands_ = new QComboBox(this);
  populateDBusCommands();
  dbusCommands_->setGeometry(QRect(770, 280, 251, 36));
  connect(dbusCommands_, SIGNAL(currentIndexChanged(int)),
      this, SLOT(updateDBusCommand(int)));

  iconLabel_ = new QLabel(this);
  iconLabel_->setText(i18n("Icon"));
  iconLabel_->setGeometry(QRect(660, 450, 72, 22));
  icon_ = new KIconButton(this);
  icon_->setGeometry(QRect(770, 417, 101, 91));

  buttonBox_ = new QDialogButtonBox(this);
  buttonBox_->setGeometry(QRect(360, 550, 341, 32));
  buttonBox_->setOrientation(Qt::Horizontal);
  buttonBox_->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Ok
      |QDialogButtonBox::Cancel);
  connect(buttonBox_, SIGNAL(accepted()),
      parent_, SLOT(updateLauncherConfig()));
  connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
  connect(buttonBox_, SIGNAL(clicked(QAbstractButton*)), this,
      SLOT(buttonClicked(QAbstractButton*)));
}

void EditLaunchersDialog::addLauncher(const QString& name,
    const QString& command, const QString& iconName) {
  name_->setText(name);
  command_->setText(command);
  icon_->setIcon(iconName);
  QListWidgetItem* item = new QListWidgetItem(getListItemIcon(iconName), name);
  item->setData(Qt::UserRole, QVariant::fromValue(
      LauncherInfo(iconName, command)));
  launchers_->addItem(item);
  launchers_->setCurrentItem(item);
}

void EditLaunchersDialog::buttonClicked(QAbstractButton* button) {
  auto role = buttonBox_->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    parent_->applyLauncherConfig();
  }
}

void EditLaunchersDialog::refreshSelectedLauncher(QListWidgetItem* current,
    QListWidgetItem* previous) {
  if (current != nullptr) {
    name_->setText(current->text());
    LauncherInfo info = current->data(Qt::UserRole).value<LauncherInfo>();
    command_->setText(info.command);
    icon_->setIcon(info.iconName);
  }
}

void EditLaunchersDialog::addLauncher() {
  addLauncher(i18n("New Launcher"), "", "xorg");
  resetInternalAndDBusCommands();
}

void EditLaunchersDialog::removeSelectedLauncher() {
  QListWidgetItem* item = launchers_->takeItem(launchers_->currentRow());
  if (item != nullptr) {
    delete item;
  }
}

void EditLaunchersDialog::updateSelectedLauncher() {
  QListWidgetItem* item = launchers_->currentItem();
  if (item != nullptr) {
    item->setText(name_->text());
    item->setIcon(getListItemIcon(icon_->icon()));
    item->setData(Qt::UserRole, QVariant::fromValue(
        LauncherInfo(icon_->icon(), command_->text())));
  }
}

void EditLaunchersDialog::browseCommand() {
  const QString& command = QFileDialog::getOpenFileName(
      this,
      i18n("Browse Command"),
      QDir::homePath());
  if (!command.isEmpty()) {
    command_->setText(command);
    resetInternalAndDBusCommands();
  }
}

void EditLaunchersDialog::updateInternalCommand(int index) {
  if (index > 0) {  // Excludes header.
    name_->setText(internalCommands_->itemText(index));
    LauncherInfo info =
        internalCommands_->itemData(index).value<LauncherInfo>();
    command_->setText(info.command);
    icon_->setIcon(info.iconName);

    dbusCommands_->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::updateDBusCommand(int index) {
  if (index > 0) {  // Excludes header.
    name_->setText(dbusCommands_->itemText(index));
    LauncherInfo info =
        dbusCommands_->itemData(index).value<LauncherInfo>();
    command_->setText(info.command);
    icon_->setIcon(info.iconName);

    internalCommands_->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::populateInternalCommands() {
  internalCommands_->addItem(i18n("Use an internal command"));  // header
  internalCommands_->addItem(i18n("Show Desktop"), QVariant::fromValue(
      LauncherInfo("user-desktop", kShowDesktopCommand)));
}

void EditLaunchersDialog::populateDBusCommands() {
  static const int kNumItems = 8;
  static const char* const kItems[kNumItems][3] = {
    // Name, icon, D-Bus command.
    {"Lock Screen",
      "system-lock-screen",
      "qdbus org.kde.screensaver /ScreenSaver Lock"},
    {"System - Log Out",
      "system-log-out",
      "qdbus org.kde.ksmserver /KSMServer logout 1 0 3"},
    {"System - Switch User",
      "system-switch-user",
      "qdbus org.kde.ksmserver /KSMServer openSwitchUserDialog"},
    {"System - Suspend",
      "system-suspend",
      "qdbus org.kde.Solid.PowerManagement /org/freedesktop/PowerManagement "
      "Suspend"},
    {"System - Hibernate",
      "system-suspend-hibernate",
      "qdbus org.kde.Solid.PowerManagement /org/freedesktop/PowerManagement "
      "Hibernate"},
    {"System - Reboot",
      "system-reboot",
      "qdbus org.kde.ksmserver /KSMServer logout 1 1 3"},
    {"System - Shut Down",
      "system-shutdown",
      "qdbus org.kde.ksmserver /KSMServer logout 1 2 3"},
    {"Activate K Menu",
      "start-here-kde",
      "qdbus org.kde.plasmashell /PlasmaShell activateLauncherMenu"}
  };
  dbusCommands_->addItem(i18n("Use a D-Bus command"));  // header
  for (int i = 0; i < kNumItems; ++i) {
    dbusCommands_->addItem(i18n(kItems[i][0]), QVariant::fromValue(
        LauncherInfo(kItems[i][1], kItems[i][2])));
  }
}

}  // namespace ksmoothdock
