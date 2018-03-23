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
#include "ui_edit_launchers_dialog.h"

#include <QFileDialog>
#include <QMimeData>
#include <QUrl>
#include <QVariant>
#include <Qt>

#include <KDesktopFile>
#include <KIconLoader>
#include <KLocalizedString>

#include "command_utils.h"
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
    const QString command = filterFieldCodes(
        desktopFile.entryMap("Desktop Entry")["Exec"]);
    const QString iconName = desktopFile.readIcon();
    parent_->addLauncher(name, command, iconName);
  } else {  // Internal drag-and-drop.
    QListWidget::dropEvent(event);
  }
}

EditLaunchersDialog::EditLaunchersDialog(QWidget* parent, MultiDockModel* model,
                                         int dockId)
    : QDialog(parent),
      ui(new Ui::EditLaunchersDialog),
      model_(model),
      dockId_(dockId) {
  ui->setupUi(this);

  qRegisterMetaType<LauncherInfo>();
  qRegisterMetaTypeStreamOperators<LauncherInfo>("LauncherInfo");

  launchers_ = new LauncherList(this);
  launchers_->setGeometry(QRect(20, 20, 351, 451));
  connect(
      launchers_,
      SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
      this,
      SLOT(refreshSelectedLauncher(QListWidgetItem*, QListWidgetItem*)));
  connect(
      launchers_,
      SIGNAL(currentRowChanged(int)),
      this,
      SLOT(resetCommandLists()));
  launchers_->setSelectionMode(QAbstractItemView::SingleSelection);
  launchers_->setDragEnabled(true);
  launchers_->setAcceptDrops(true);
  launchers_->setDropIndicatorShown(true);
  launchers_->setDragDropMode(QAbstractItemView::DragDrop);

  connect(ui->launchersNote, SIGNAL(linkActivated(const QString&)),
          this, SLOT(openLink(const QString&)));
  connect(ui->add, SIGNAL(clicked()), this, SLOT(addLauncher()));
  connect(ui->remove, SIGNAL(clicked()), this, SLOT(removeSelectedLauncher()));
  connect(ui->update, SIGNAL(clicked()), this, SLOT(updateSelectedLauncher()));
  connect(ui->command, SIGNAL(textEdited(const QString&)),
      this, SLOT(resetCommandLists()));
  connect(ui->browseCommand, SIGNAL(clicked()),
      this, SLOT(browseCommand()));

  populateInternalCommands();
  connect(ui->internalCommands, SIGNAL(currentIndexChanged(int)),
      this, SLOT(updateInternalCommand(int)));
  populateDBusCommands();
  connect(ui->dbusCommands, SIGNAL(currentIndexChanged(int)),
      this, SLOT(updateDBusCommand(int)));
  populateWebCommands();
  connect(ui->webCommands, SIGNAL(currentIndexChanged(int)),
      this, SLOT(updateWebCommand(int)));

  icon_ = new KIconButton(this);
  icon_->setGeometry(QRect(680, 390, 80, 80));

  connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this,
      SLOT(buttonClicked(QAbstractButton*)));

  loadData();
}

void EditLaunchersDialog::addLauncher(const QString& name,
    const QString& command, const QString& iconName) {
  ui->name->setText(name);
  ui->command->setText(command);
  icon_->setIcon(iconName);
  QListWidgetItem* item = new QListWidgetItem(getListItemIcon(iconName), name);
  item->setData(Qt::UserRole, QVariant::fromValue(
      LauncherInfo(iconName, command)));
  launchers_->addItem(item);
  launchers_->setCurrentItem(item);
}

void EditLaunchersDialog::accept() {
  QDialog::accept();
  saveData();
}

void EditLaunchersDialog::buttonClicked(QAbstractButton* button) {
  auto role = ui->buttonBox->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    saveData();
  }
}

void EditLaunchersDialog::refreshSelectedLauncher(QListWidgetItem* current,
    QListWidgetItem* previous) {
  if (current != nullptr) {
    ui->name->setText(current->text());
    LauncherInfo info = current->data(Qt::UserRole).value<LauncherInfo>();
    ui->command->setText(info.command);
    icon_->setIcon(info.iconName);
  }
}

void EditLaunchersDialog::addLauncher() {
  addLauncher(i18n("New Launcher"), "", "xorg");
  resetCommandLists();
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
    item->setText(ui->name->text());
    item->setIcon(getListItemIcon(icon_->icon()));
    item->setData(Qt::UserRole, QVariant::fromValue(
        LauncherInfo(icon_->icon(), ui->command->text())));
  }
}

void EditLaunchersDialog::openLink(const QString& link) {
  Launcher::launch("xdg-open " + link);
}

void EditLaunchersDialog::browseCommand() {
  const QString& command = QFileDialog::getOpenFileName(
      this,
      i18n("Browse Command"),
      QDir::homePath());
  if (!command.isEmpty()) {
    ui->command->setText(command);
    resetCommandLists();
  }
}

void EditLaunchersDialog::updateInternalCommand(int index) {
  if (index > 0) {  // Excludes header.
    ui->name->setText(ui->internalCommands->itemText(index));
    LauncherInfo info =
        ui->internalCommands->itemData(index).value<LauncherInfo>();
    ui->command->setText(info.command);
    icon_->setIcon(info.iconName);

    ui->dbusCommands->setCurrentIndex(0);
    ui->webCommands->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::updateDBusCommand(int index) {
  if (index > 0) {  // Excludes header.
    ui->name->setText(ui->dbusCommands->itemText(index));
    LauncherInfo info =
        ui->dbusCommands->itemData(index).value<LauncherInfo>();
    ui->command->setText(info.command);
    icon_->setIcon(info.iconName);

    ui->internalCommands->setCurrentIndex(0);
    ui->webCommands->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::updateWebCommand(int index) {
  if (index > 0) {  // Excludes header.
    ui->name->setText(ui->webCommands->itemText(index));
    LauncherInfo info =
        ui->webCommands->itemData(index).value<LauncherInfo>();
    ui->command->setText(info.command);
    icon_->setIcon(info.iconName);

    ui->internalCommands->setCurrentIndex(0);
    ui->dbusCommands->setCurrentIndex(0);
  }
}

void EditLaunchersDialog::resetCommandLists() {
  ui->internalCommands->setCurrentIndex(0);
  ui->dbusCommands->setCurrentIndex(0);
  ui->webCommands->setCurrentIndex(0);
}

void EditLaunchersDialog::loadData() {
  launchers_->clear();
  for (const auto& item : model_->dockLauncherConfigs(dockId_)) {
    QPixmap icon = KIconLoader::global()->loadIcon(
        item.icon, KIconLoader::NoGroup, kListIconSize);
    QListWidgetItem* listItem = new QListWidgetItem(
          icon, item.name);
    listItem->setData(Qt::UserRole, QVariant::fromValue(
                        LauncherInfo(item.icon, item.command)));
    launchers_->addItem(listItem);
  }
}

void EditLaunchersDialog::saveData() {
  const int launcherCount = launchers_->count();
  std::vector<LauncherConfig> launcherConfigs;
  launcherConfigs.reserve(launcherCount);
  for (int i = 0; i < launcherCount; ++i) {
    auto* listItem = launchers_->item(i);
    auto info = listItem->data(Qt::UserRole).value<LauncherInfo>();
    launcherConfigs.push_back(LauncherConfig(
                                listItem->text(), info.iconName, info.command));
  }
  model_->setDockLauncherConfigs(dockId_, launcherConfigs);
  model_->saveDockLauncherConfigs(dockId_);
}

void EditLaunchersDialog::populateInternalCommands() {
  ui->internalCommands->addItem(i18n("Use an internal command"));  // header
  ui->internalCommands->addItem(
      getListItemIcon("user-desktop"),
      i18n("Show Desktop"),
      QVariant::fromValue(LauncherInfo("user-desktop", kShowDesktopCommand)));
}

void EditLaunchersDialog::populateDBusCommands() {
  static const int kNumItems = 7;
  static const char* const kItems[kNumItems][3] = {
    // Name, icon, D-Bus command.
    {"System - Lock Screen",
      "system-lock-screen",
      "qdbus org.kde.screensaver /ScreenSaver Lock"},
    {"System - Log Out",
      "system-log-out",
      "qdbus org.kde.ksmserver /KSMServer logout -1 0 3"},
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
      "qdbus org.kde.ksmserver /KSMServer logout -1 1 3"},
    {"System - Shut Down",
      "system-shutdown",
      "qdbus org.kde.ksmserver /KSMServer logout -1 2 3"}
  };
  ui->dbusCommands->addItem(i18n("Use a D-Bus command"));  // header
  for (int i = 0; i < kNumItems; ++i) {
    ui->dbusCommands->addItem(
        getListItemIcon(kItems[i][1]),
        i18n(kItems[i][0]),
        QVariant::fromValue(LauncherInfo(kItems[i][1], kItems[i][2])));
  }
}

void EditLaunchersDialog::populateWebCommands() {
  static const int kNumItems = 25;
  static const char* const kItems[kNumItems][3] = {
    // Name, icon, command.
    {"Google",
      "applications-internet",
      "xdg-open https://www.google.com"},
    {"Google (Chrome New Window)",
      "applications-internet",
      "google-chrome --new-window https://www.google.com"},
    {"Google (Chrome Incognito Window)",
      "applications-internet",
      "google-chrome --new-window --incognito https://www.google.com"},
    {"Google (Firefox New Window)",
      "applications-internet",
      "firefox --new-window https://www.google.com"},
    {"Google (Firefox Private Window)",
      "applications-internet",
      "firefox --private-window https://www.google.com"},
    {"Gmail",
      "internet-mail",
      "xdg-open https://www.gmail.com"},
    {"Gmail (Chrome New Window)",
      "internet-mail",
      "google-chrome --new-window https://www.gmail.com"},
    {"Gmail (Chrome Incognito Window)",
      "internet-mail",
      "google-chrome --new-window --incognito https://www.gmail.com"},
    {"Gmail (Firefox New Window)",
      "internet-mail",
      "firefox --new-window https://www.gmail.com"},
    {"Gmail (Firefox Private Window)",
      "internet-mail",
      "firefox --private-window https://www.gmail.com"},
    {"YouTube",
      "applications-multimedia",
      "xdg-open https://www.youtube.com"},
    {"YouTube (Chrome New Window)",
      "applications-multimedia",
      "google-chrome --new-window https://www.youtube.com"},
    {"YouTube (Chrome Incognito Window)",
      "applications-multimedia",
      "google-chrome --new-window --incognito https://www.youtube.com"},
    {"YouTube (Firefox New Window)",
      "applications-multimedia",
      "firefox --new-window https://www.youtube.com"},
    {"YouTube (Firefox Private Window)",
      "applications-multimedia",
      "firefox --private-window https://www.youtube.com"},
    {"Facebook",
      "system-users",
      "xdg-open https://www.facebook.com"},
    {"Facebook (Chrome New Window)",
      "system-users",
      "google-chrome --new-window https://www.facebook.com"},
    {"Facebook (Chrome Incognito Window)",
      "system-users",
      "google-chrome --new-window --incognito https://www.facebook.com"},
    {"Facebook (Firefox New Window)",
      "system-users",
      "firefox --new-window https://www.facebook.com"},
    {"Facebook (Firefox Private Window)",
      "system-users",
      "firefox --private-window https://www.facebook.com"},
    {"Twitter",
      "system-users",
      "xdg-open https://www.twitter.com"},
    {"Twitter (Chrome New Window)",
      "system-users",
      "google-chrome --new-window https://www.twitter.com"},
    {"Twitter (Chrome Incognito Window)",
      "system-users",
      "google-chrome --new-window --incognito https://www.twitter.com"},
    {"Twitter (Firefox New Window)",
      "system-users",
      "firefox --new-window https://www.twitter.com"},
    {"Twitter (Firefox Private Window)",
      "system-users",
      "firefox --private-window https://www.twitter.com"},
  };
  ui->webCommands->addItem(i18n("Launch a Website"));  // header
  for (int i = 0; i < kNumItems; ++i) {
    ui->webCommands->addItem(
        getListItemIcon(kItems[i][1]),
        i18n(kItems[i][0]),
        QVariant::fromValue(LauncherInfo(kItems[i][1], kItems[i][2])));
  }
}

}  // namespace ksmoothdock
