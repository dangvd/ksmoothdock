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

#ifndef KSMOOTHDOCK_ICON_OVERRIDE_RULES_DIALOG_H_
#define KSMOOTHDOCK_ICON_OVERRIDE_RULES_DIALOG_H_

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

#include <model/multi_dock_model.h>

namespace Ui {
  class IconOverrideRulesDialog;
}

namespace ksmoothdock {

// User data for the items in QListWidget/QComboBox.
struct RuleInfo {
  // The name(label) is already stored as item text in QListWidget/QComboBox.
  QString icon;
  QString window_name_regex;

  RuleInfo() {}
  RuleInfo(QString icon2, QString window_name_regex2)
      : icon(icon2), window_name_regex(window_name_regex2) {}
};

QDataStream &operator<<(QDataStream &out, const RuleInfo& rule);
QDataStream &operator>>(QDataStream &in, RuleInfo& rule);

class IconOverrideRulesDialog;

class RuleList : public QListWidget {
 public:
  explicit RuleList(IconOverrideRulesDialog* parent);

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;

 private:
  IconOverrideRulesDialog* parent_;
};

class IconOverrideRulesDialog : public QDialog {
  Q_OBJECT

 public:
  IconOverrideRulesDialog(QWidget* parent, MultiDockModel* model);
  ~IconOverrideRulesDialog() = default;

  void reload() { loadData(); }

  void addRule(const QString& name, const QString& window_name_regex,
      const QString& icon);

 public slots:
  void accept() override;
  void buttonClicked(QAbstractButton* button);

  void refreshSelectedRule(QListWidgetItem* current,
      QListWidgetItem* previous);

  void addRule();
  void removeSelectedRule();
  void removeAllRules();
  void updateSelectedRule();

  void resetRulePresets();

 private:
  static constexpr int kListIconSize = 48;

  void loadData();
  void saveData();

  QIcon getListItemIcon(const QString& icon) {
    return QIcon(KIconLoader::global()->loadIcon(icon,
        KIconLoader::NoGroup, kListIconSize));
  }

  void populateRulePresets();

  void clearItemDetails();

  Ui::IconOverrideRulesDialog *ui;
  RuleList *rules_;
  KIconButton *icon_;

  MultiDockModel* model_;

  friend class IconOverrideRulesDialogTest;
};

}  // namespace ksmoothdock

Q_DECLARE_METATYPE(ksmoothdock::RuleInfo);

#endif  // KSMOOTHDOCK_ICON_OVERRIDE_RULES_DIALOG_H_
