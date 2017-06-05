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

#include "application_menu.h"

#include <algorithm>

#include <QApplication>
#include <QDir>
#include <QDrag>
#include <QMimeData>
#include <QStringBuilder>
#include <QUrl>

#include <KConfigGroup>
#include <KDesktopFile>
#include <KIconLoader>
#include <KLocalizedString>
#include <KWindowSystem>

#include "ksmoothdock.h"
#include "launcher.h"

namespace ksmoothdock {

const std::vector<Category> ApplicationMenu::kSessionSystemCategories = {
  {"Session", "Session", "system-switch-user", {
    {"Lock Screen",
      "",
      "system-lock-screen",
      "qdbus org.kde.screensaver /ScreenSaver Lock",
      ""},
    {"Log Out",
      "",
      "system-log-out",
      "qdbus org.kde.ksmserver /KSMServer logout -1 0 3",
      ""},
    {"Switch User",
      "",
      "system-switch-user",
      "qdbus org.kde.ksmserver /KSMServer openSwitchUserDialog",
      ""}
    }
  },
  {"Power", "Power", "system-shutdown", {
    {"Suspend",
      "",
      "system-suspend",
      "qdbus org.kde.Solid.PowerManagement /org/freedesktop/PowerManagement "
      "Suspend",
      ""},
    {"Hibernate",
      "",
      "system-suspend-hibernate",
      "qdbus org.kde.Solid.PowerManagement /org/freedesktop/PowerManagement "
      "Hibernate",
      ""},
    {"Reboot",
      "",
      "system-reboot",
      "qdbus org.kde.ksmserver /KSMServer logout -1 1 3",
      ""},
    {"Shut Down",
      "",
      "system-shutdown",
      "qdbus org.kde.ksmserver /KSMServer logout -1 2 3",
      ""}
    }
  }
};
const ApplicationEntry ApplicationMenu::kSearchEntry = {
  "Search", "", "system-search", "krunner", ""
};

int ApplicationMenuStyle::pixelMetric(
    PixelMetric metric, const QStyleOption *option, const QWidget *widget)
    const {
  if (metric == QStyle::PM_SmallIconSize) {
    return kApplicationMenuIconSize;
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

bool operator<(const ApplicationEntry &e1, const ApplicationEntry &e2) {
  return e1.name < e2.name;
}

ApplicationMenu::ApplicationMenu(
    KSmoothDock *parent, Qt::Orientation orientation, int minSize, int maxSize,
    KConfig *config, const QStringList& entryDirs)
    : IconBasedDockItem(parent, "" /* label */, orientation, "" /* iconName */,
                        minSize, maxSize),
      config_(config),
      entryDirs_(entryDirs),
      fileWatcher_(entryDirs),
      configDialog_(parent, this) {}

void ApplicationMenu::init() {
  menu_.setStyle(&style_);
  menu_.setStyleSheet(getStyleSheet());

  loadConfig();
  initCategories();
  loadEntries();
  buildMenu();

  connect(&menu_, SIGNAL(aboutToShow()), parent_,
          SLOT(setStrutForApplicationMenu()));
  connect(&menu_, SIGNAL(aboutToHide()), parent_, SLOT(setStrut()));
  connect(&fileWatcher_, SIGNAL(directoryChanged(const QString&)),
          this, SLOT(reloadMenu()));
  connect(&fileWatcher_, SIGNAL(fileChanged(const QString&)),
          this, SLOT(reloadMenu()));
}

void ApplicationMenu::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    menu_.popup(parent_->getApplicationMenuPosition(getMenuSize()));
  }
}

void ApplicationMenu::reloadMenu() {
  for (auto& category : categories_) {
    category.entries.clear();
  }
  loadEntries();
  menu_.clear();
  buildMenu();
}

void ApplicationMenu::showConfigDialog() {
  configDialog_.name_->setText(label_);
  configDialog_.icon_->setIcon(iconName_);
  configDialog_.show();
}

void ApplicationMenu::applyConfig() {
  setLabel(configDialog_.name_->text());
  setIconName(configDialog_.icon_->icon());
  saveConfig();
  parent_->refresh();
}

void ApplicationMenu::updateConfig() {
  applyConfig();
  configDialog_.hide();
}

bool ApplicationMenu::eventFilter(QObject* object, QEvent* event) {
  QMenu* menu = dynamic_cast<QMenu*>(object);
  if (menu) {
    if (event->type() == QEvent::Show) {
      menu->popup(parent_->getApplicationSubMenuPosition(getMenuSize(),
                                                         menu->geometry()));
      // Filter this event.
      return true;
    } else if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
      if (mouseEvent && mouseEvent->button() == Qt::LeftButton
          && menu->activeAction()) {
        startMousePos_ = mouseEvent->pos();
        draggedEntry_ = menu->activeAction()->data().toString();
      }
    } else if (event->type() == QEvent::MouseMove) {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
      if (mouseEvent && mouseEvent->buttons() & Qt::LeftButton) {
        const int distance
            = (mouseEvent->pos() - startMousePos_).manhattanLength();
        if (distance >= QApplication::startDragDistance()
            && !draggedEntry_.isEmpty()) {
          // Start drag.
          QMimeData* mimeData = new QMimeData;
          mimeData->setData("text/uri-list",
                            QUrl::fromLocalFile(draggedEntry_).toEncoded());

          QDrag* drag = new QDrag(this);
          drag->setMimeData(mimeData);
          drag->exec(Qt::CopyAction);
        }
      }
    }
  }

  return QObject::eventFilter(object, event);
}

QString ApplicationMenu::getStyleSheet() {
  QColor bgColor = parent_->getBackgroundColor();
  QColor borderColor = parent_->getBorderColor();
  return " \
QMenu { \
  background-color: " % bgColor.name(QColor::HexArgb) % ";"
" margin: 1px; \
  padding: 2px; \
  border: 1px transparent; \
  border-radius: 3px; \
} \
\
QMenu::item { \
  font: bold; \
  color: white; \
  background-color: transparent; \
  padding: 4px 45px 4px 45px; \
} \
\
QMenu::item:selected { \
  background-color: " % bgColor.name(QColor::HexArgb) % ";"
" border: 1px solid " % borderColor.name() % ";"
" border-radius: 3px; \
} \
\
QMenu::separator { \
  margin: 5px; \
  height: 1px; \
  background: " % borderColor.name() % ";"
"}";
}

void ApplicationMenu::loadConfig() {
  KConfigGroup group(config_, "Application Menu");
  setLabel(group.readEntry("label", i18n("Applications")));
  setIconName(group.readEntry("icon", "start-here-kde"));
}

void ApplicationMenu::saveConfig() {
  KConfigGroup group(config_, "Application Menu");
  group.writeEntry("label", label_);
  group.writeEntry("icon", iconName_);
  config_->sync();
}

void ApplicationMenu::initCategories() {
  // We use the main categories as defined in:
  // https://specifications.freedesktop.org/menu-spec/latest/apa.html
  static constexpr int kNumCategories = 11;
  static const char* const kCategories[kNumCategories][3] = {
    // Name, display name, icon.
    // Sorted by display name.
    {"Development", "Development", "applications-development"},
    {"Education", "Education", "applications-education"},
    {"Game", "Games", "applications-games"},
    {"Graphics", "Graphics", "applications-graphics"},
    {"Network", "Internet", "applications-internet"},
    {"AudioVideo", "Multimedia", "applications-multimedia"},
    {"Office", "Office", "applications-office"},
    {"Science", "Science", "applications-science"},
    {"Settings", "Settings", "preferences-other"},
    {"System", "System", "applications-system"},
    {"Utility", "Utilities", "applications-utilities"},
  };
  categories_.reserve(kNumCategories);
  for (int i = 0; i < kNumCategories; ++i) {
    categories_.push_back(Category(
        kCategories[i][0], kCategories[i][1], kCategories[i][2]));
    categoryMap_[kCategories[i][0]] = i;
  }
}

bool ApplicationMenu::loadEntries() {
  for (const QString& entryDir : entryDirs_) {
    if (!QDir::root().exists(entryDir)) {
      continue;
    }

    QDir dir(entryDir);
    QStringList files = dir.entryList({"*.desktop"}, QDir::Files, QDir::Name);
    if (files.isEmpty()) {
      continue;
    }

    for (int i = 0; i < files.size(); ++i) {
      const QString& file = entryDir + "/" + files.at(i);
      loadEntry(file);
    }
  }

  return true;
}

bool ApplicationMenu::loadEntry(const QString &file) {
  KDesktopFile desktopFile(file);
  if (desktopFile.noDisplay()) {
    return false;
  }

  const QString hidden = desktopFile.entryMap("Desktop Entry")["Hidden"];
  if (!hidden.isEmpty() && hidden.trimmed().toLower() == "true") {
    return false;
  }

  const QStringList categories =
      desktopFile.entryMap("Desktop Entry")["Categories"]
          .split(';', QString::SkipEmptyParts);
  if (categories.isEmpty()) {
    return false;
  }

  for (int i = 0; i < categories.size(); ++i) {
    const std::string category = categories[i].toStdString();
    if (categoryMap_.count(category) > 0) {
      const QString command = Launcher::filterFieldCodes(
          desktopFile.entryMap("Desktop Entry")["Exec"]);
      ApplicationEntry newEntry(desktopFile.readName(),
                                desktopFile.readGenericName(),
                                desktopFile.readIcon(),
                                command,
                                file);
      auto& entries = categories_[categoryMap_[category]].entries;
      auto next = std::lower_bound(entries.begin(), entries.end(), newEntry);
      entries.insert(next, newEntry);
    }
  }
  return true;
}

void ApplicationMenu::buildMenu() {
  addToMenu(categories_);
  menu_.addSeparator();
  addToMenu(kSessionSystemCategories);
  addEntry(kSearchEntry, &menu_);
}

void ApplicationMenu::addToMenu(const std::vector<Category>& categories) {
  for (const auto& category : categories) {
    if (category.entries.empty()) {
      continue;
    }

    QMenu* menu = menu_.addMenu(loadIcon(category.icon), category.displayName);
    menu->setStyle(&style_);
    for (const auto& entry : category.entries) {
      addEntry(entry, menu);
    }
    menu->installEventFilter(this);
  }
}

void ApplicationMenu::addEntry(const ApplicationEntry &entry, QMenu *menu) {
  QAction* action = menu->addAction(loadIcon(entry.icon), entry.name, this,
                  [this, &entry]() {
                    Launcher::launch(entry.command);
                  });
  action->setData(entry.desktopFile);
}

QIcon ApplicationMenu::loadIcon(const QString &icon) {
  return QIcon(KIconLoader::global()->loadIcon(
      icon, KIconLoader::NoGroup, kApplicationMenuIconSize));
}

}  // namespace ksmoothdock
