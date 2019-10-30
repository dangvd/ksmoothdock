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
  return fs::is_symlink(appCommand) ?
      fs::path(fs::read_symlink(appCommand)).filename() :
      fs::path(appCommand).filename();
}

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_COMMAND_UTILS_H_
