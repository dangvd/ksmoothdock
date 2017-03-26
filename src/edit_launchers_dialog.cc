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
    const QString command = desktopFile.entryMap("Desktop Entry")["Exec"];
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
  add_->setGeometry(QRect(500, 50, 121, 38));
  connect(add_, SIGNAL(clicked()), this, SLOT(addLauncher()));

  remove_ = new QPushButton(this);
  remove_->setText(i18n("Remove"));
  remove_->setIcon(QIcon::fromTheme("list-remove"));
  remove_->setGeometry(QRect(500, 110, 121, 38));
  connect(remove_, SIGNAL(clicked()), this, SLOT(removeSelectedLauncher()));

  update_ = new QPushButton(this);
  update_->setText(i18n("Update"));
  update_->setIcon(QIcon::fromTheme("arrow-left"));
  update_->setGeometry(QRect(500, 170, 121, 38));
  connect(update_, SIGNAL(clicked()), this, SLOT(updateSelectedLauncher()));

  openDir_ = new QPushButton(this);
  openDir_->setText(i18n("Open Folder"));
  openDir_->setGeometry(QRect(500, 300, 121, 38));
  connect(openDir_, SIGNAL(clicked()), parent_, SLOT(openLaunchersDir()));
  syncFromDir_ = new QPushButton(this);
  syncFromDir_->setText(i18n("Sync From\nFolder"));
  syncFromDir_->setGeometry(QRect(500, 360, 121, 61));

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
  connect(buttonBox_, SIGNAL(accepted()), parent_, SLOT(updateConfig()));
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
    parent_->applyConfig();
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
  }
}

void EditLaunchersDialog::updateInternalCommand(int index) {
  if (index > 0) {  // Excludes header.
    command_->setText(internalCommands_->itemData(index).toString());
    dbusCommands_->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::updateDBusCommand(int index) {
  if (index > 0) {  // Excludes header.
    command_->setText(dbusCommands_->itemData(index).toString());
    internalCommands_->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::populateInternalCommands() {
  internalCommands_->addItem(i18n("Use an internal command"));  // header
  internalCommands_->addItem(i18n("Show desktop"), "SHOW_DESKTOP");
}

void EditLaunchersDialog::populateDBusCommands() {
  static const int kNumItems = 3;
  static const char* const kItems[kNumItems][2] = {
    // Description, D-Bus command.
    {"Lock the screen",
      "qdbus org.kde.screensaver /ScreenSaver Lock"},
    {"Suspend the computer",
      "qdbus org.kde.Solid.PowerManagement /org/freedesktop/PowerManagement "
      "Suspend"},
    {"Show the application menu",
      "qdbus org.kde.plasmashell /PlasmaShell activateLauncherMenu"}
  };
  dbusCommands_->addItem(i18n("Use a D-Bus command"));  // header
  for (int i = 0; i < kNumItems; ++i) {
    dbusCommands_->addItem(i18n(kItems[i][0]), kItems[i][1]);
  }
}

}  // namespace ksmoothdock
