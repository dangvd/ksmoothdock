/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2018 Viet Dang (dangvd@gmail.com)
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

#ifndef KSMOOTHDOCK_COMMAND_UTILS_H_
#define KSMOOTHDOCK_COMMAND_UTILS_H_

#include <filesystem>

#include <QString>

namespace ksmoothdock {

static constexpr char kShowDesktopCommand[] = "SHOW_DESKTOP";
static constexpr char kLockScreenCommand[] =
    "qdbus org.kde.screensaver /ScreenSaver Lock";

inline QString filterFieldCodes(const QString& command) {
  if (command.contains('%')) {
    return command.left(command.indexOf(' '));
  }
  return command;
}

inline bool isCommandInternal(const QString& command) {
  return command == kShowDesktopCommand;
}

inline bool isCommandDBus(const QString& command) {
  return command.startsWith("qdbus");
}

inline bool isCommandLockScreen(const QString& command) {
  return command == kLockScreenCommand;
}

inline std::string getTaskCommand(const std::string& appCommand) {
  namespace fs = std::filesystem;
  const auto command = appCommand.substr(0, appCommand.find_first_of(' '));
  return fs::is_symlink(command) ?
      fs::path(fs::read_symlink(command)).filename() :
      fs::path(command).filename();
}

inline QString getTaskCommand(const QString& appCommand) {
  return QString::fromStdString(getTaskCommand(appCommand.toStdString()));
}

inline bool areTheSameCommand(const QString& appTaskCommand, const QString& taskCommand) {
  // Fix for Firefox and Thunderbird.
  if (taskCommand == "navigator" && (appTaskCommand == "firefox" || appTaskCommand == "firefox-esr")) {
    return true;
  }
  if (taskCommand == "mail" && appTaskCommand == "thunderbird") {
    return true;
  }
  return appTaskCommand == taskCommand;
}

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_COMMAND_UTILS_H_
