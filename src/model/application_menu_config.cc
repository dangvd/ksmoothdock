/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2019 Viet Dang (dangvd@gmail.com)
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

#include "application_menu_config.h"

#include <algorithm>

#include <QApplication>
#include <QDir>
#include <QStringBuilder>
#include <QUrl>

#include <KDesktopFile>
#include <KLocalizedString>

#include <utils/command_utils.h>

namespace ksmoothdock {

const std::vector<Category> ApplicationMenuConfig::kSessionSystemCategories = {
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
const ApplicationEntry ApplicationMenuConfig::kSearchEntry = {
  "Search", "", "system-search", "krunner", ""
};

bool operator<(const ApplicationEntry &e1, const ApplicationEntry &e2) {
  return e1.name < e2.name;
}

ApplicationMenuConfig::ApplicationMenuConfig(const QStringList& entryDirs)
    : entryDirs_(entryDirs),
      fileWatcher_(entryDirs) {
  initCategories();
  loadEntries();
  connect(&fileWatcher_, SIGNAL(directoryChanged(const QString&)),
          this, SLOT(reload()));
  connect(&fileWatcher_, SIGNAL(fileChanged(const QString&)),
          this, SLOT(reload()));
}

void ApplicationMenuConfig::initCategories() {
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

bool ApplicationMenuConfig::loadEntries() {
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

bool ApplicationMenuConfig::loadEntry(const QString &file) {
  KDesktopFile desktopFile(file);
  if (desktopFile.noDisplay()) {
    return false;
  }

  if (desktopFile.entryMap("Desktop Entry").contains("Hidden")) {
    const QString hidden = desktopFile.entryMap("Desktop Entry")["Hidden"];
    if (hidden.trimmed().toLower() == "true") {
      return false;
    }
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
      const QString command = filterFieldCodes(
          desktopFile.entryMap("Desktop Entry")["Exec"]);
      ApplicationEntry newEntry(desktopFile.readName(),
                                desktopFile.readGenericName(),
                                desktopFile.readIcon(),
                                command,
                                file);
      auto& entries = categories_[categoryMap_[category]].entries;
      auto next = std::lower_bound(entries.begin(), entries.end(), newEntry);
      entries.insert(next, newEntry);

      entries_[command.toStdString()] = &(*--next);
    }
  }
  return true;
}

void ApplicationMenuConfig::reload() {
  for (auto& category : categories_) {
    category.entries.clear();
  }
  loadEntries();
  emit configChanged();
}

const ApplicationEntry* ApplicationMenuConfig::findApplication(
    const std::string& command) const {
  return (entries_.count(command) > 0) ? entries_.at(command) : nullptr;
}

}  // namespace ksmoothdock
