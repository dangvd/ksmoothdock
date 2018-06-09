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

#ifndef KSMOOTHDOCK_ICON_OVERRIDE_RULE_H_
#define KSMOOTHDOCK_ICON_OVERRIDE_RULE_H_

#include <iostream>

#include <QFile>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace ksmoothdock {

struct IconOverrideRule {
  static constexpr char DELIM = '\t';

  QString name;
  QString window_name_regex;
  QString icon;

  IconOverrideRule() = default;

  IconOverrideRule(const QString& name2, const QString& window_name_regex2,
                   const QString& icon2)
      : name(name2), window_name_regex(window_name_regex2), icon(icon2) {}

  IconOverrideRule(const QString& s) {
    auto list = s.split(DELIM);
    if (list.size() == 3) {
      name = list[0];
      window_name_regex = list[1];
      icon = list[2];
    }
  }

  QString toString() const {
    return name + DELIM + window_name_regex + DELIM + icon;
  }
};

template <typename S>
S loadIconOverrideRulesFromFile(const QString& filename) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    std::cerr << "Failed to read icon override rules from file: "
              << filename.toStdString();
    return S{};
  }

  S rules{};
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    if (!line.isEmpty()) {
      rules.push_back(IconOverrideRule(line));
    }
  }
  return rules;
}

template <typename S>
void saveIconOverrideRulesToFile(const QString& filename, const S& s) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    std::cerr << "Failed to write icon override rules to file: "
              << filename.toStdString();
    return;
  }

  QTextStream out(&file);
  for (const auto& rule : s) {
    out << rule.toString() << "\n";
  }
}

}

#endif
