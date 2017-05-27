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

#include <QDir>

#include <KDesktopFile>
#include <KConfigGroup>
#include <KLocalizedString>

#include "launcher.h"

namespace ksmoothdock {

ApplicationMenu::ApplicationMenu(
    KSmoothDock *parent, Qt::Orientation orientation, int minSize, int maxSize,
    KConfig *config, const QString &entryDir)
    : IconBasedDockItem(parent, "" /* label */, orientation, "" /* iconName */,
                        minSize, maxSize),
      config_(config),
      entryDir_(entryDir) {
  loadConfig();
  initCategories();
  loadEntries();
  buildMenu();
}

void ApplicationMenu::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    menu_.popup(e->globalPos());
  } else if (e->button() == Qt::RightButton) {
    //menu_.popup(e->globalPos());
  }
}

void ApplicationMenu::loadConfig() {
  KConfigGroup group(config_, "Application Menu");
  setLabel(group.readEntry("label", i18n("Applications")));
  setIconName(group.readEntry("icon", "start-here-kde"));
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
  if (!QDir::root().exists(entryDir_)) {
    return false;
  }

  QDir entryDir(entryDir_);
  QStringList files = entryDir.entryList({"*.desktop"}, QDir::Files,
                                         QDir::Name);
  if (files.isEmpty()) {
    return false;
  }

  for (int i = 0; i < files.size(); ++i) {
    const QString& file = entryDir_ + "/" + files.at(i);
    loadEntry(file);
  }
  return true;
}

bool ApplicationMenu::loadEntry(const QString &file) {
  KDesktopFile desktopFile(file);
  if (desktopFile.noDisplay()) {
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
      categories_[categoryMap_[category]].entries.push_back(
          std::unique_ptr<ApplicationEntry>(new ApplicationEntry(
              desktopFile.readName(),
              desktopFile.readGenericName(),
              desktopFile.readIcon(),
              command)));
    }
  }
  return true;
}

void ApplicationMenu::buildMenu() {
  for (const auto& category : categories_) {
    QMenu* menu = menu_.addMenu(QIcon::fromTheme(category.icon),
                                category.displayName);
    for (const auto& entry : category.entries) {
      menu->addAction(QIcon::fromTheme(entry->icon), entry->name, this,
                      [this, &entry]() {
                        Launcher::launch(entry->command);
                      });
    }
  }
}

}  // namespace ksmoothdock
