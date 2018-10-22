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

#include "icon_override_rules_dialog.h"
#include "ui_icon_override_rules_dialog.h"

#include <QFileDialog>
#include <QMimeData>
#include <QUrl>
#include <QVariant>
#include <Qt>

#include <KDesktopFile>
#include <KIconLoader>
#include <KLocalizedString>

namespace ksmoothdock {

QDataStream &operator<<(QDataStream &out, const RuleInfo& rule) {
  out << rule.program << rule.window_name_regex << rule.icon;
  return out;
}

QDataStream &operator>>(QDataStream &in, RuleInfo& rule) {
  in >> rule.program >> rule.window_name_regex >> rule.icon;
  return in;
}

RuleList::RuleList(IconOverrideRulesDialog* parent)
    : QListWidget(parent), parent_(parent) {}

void RuleList::dragEnterEvent(QDragEnterEvent *event) {
  // Internal drag-and-drop.
  RuleList* source = dynamic_cast<RuleList*>(event->source());
  if (source != nullptr && source == this) {
    event->acceptProposedAction();
    setDragDropMode(QAbstractItemView::InternalMove);
    return;
  }
}

void RuleList::dragMoveEvent(QDragMoveEvent* event) {
  event->acceptProposedAction();
}

IconOverrideRulesDialog::IconOverrideRulesDialog(
    QWidget* parent, MultiDockModel* model)
    : QDialog(parent),
      ui(new Ui::IconOverrideRulesDialog),
      model_(model) {
  ui->setupUi(this);

  qRegisterMetaType<RuleInfo>();
  qRegisterMetaTypeStreamOperators<RuleInfo>("RuleInfo");

  rules_ = new RuleList(this);
  rules_->setGeometry(QRect(20, 20, 351, 451));
  connect(
      rules_,
      SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
      this,
      SLOT(refreshSelectedRule(QListWidgetItem*, QListWidgetItem*)));
  connect(
      rules_,
      SIGNAL(currentRowChanged(int)),
      this,
      SLOT(resetRulePresets()));
  rules_->setSelectionMode(QAbstractItemView::SingleSelection);
  rules_->setDragEnabled(true);
  rules_->setAcceptDrops(true);
  rules_->setDropIndicatorShown(true);
  rules_->setDragDropMode(QAbstractItemView::DragDrop);

  connect(ui->add, SIGNAL(clicked()), this, SLOT(addRule()));
  connect(ui->remove, SIGNAL(clicked()), this, SLOT(removeSelectedRule()));
  connect(ui->removeAll, SIGNAL(clicked()), this, SLOT(removeAllRules()));
  connect(ui->update, SIGNAL(clicked()), this, SLOT(updateSelectedRule()));
  connect(ui->windowNameRegex, SIGNAL(textEdited(const QString&)),
      this, SLOT(resetRuleLists()));

  icon_ = new KIconButton(this);
  icon_->setGeometry(QRect(800, 390, 80, 80));

  connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this,
      SLOT(buttonClicked(QAbstractButton*)));

  loadData();
}

void IconOverrideRulesDialog::addRule(const QString& name, const QString& program,
    const QString& window_name_regex, const QString& icon) {
  ui->name->setText(name);
  ui->program->setText(program);
  ui->windowNameRegex->setText(window_name_regex);
  icon_->setIcon(icon);
  QListWidgetItem* item = new QListWidgetItem(getListItemIcon(icon), name);
  item->setData(Qt::UserRole, QVariant::fromValue(
      RuleInfo(program, window_name_regex, icon)));
  rules_->addItem(item);
  rules_->setCurrentItem(item);
}

void IconOverrideRulesDialog::accept() {
  QDialog::accept();
  saveData();
}

void IconOverrideRulesDialog::buttonClicked(QAbstractButton* button) {
  auto role = ui->buttonBox->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    saveData();
  }
}

void IconOverrideRulesDialog::refreshSelectedRule(QListWidgetItem* current,
    QListWidgetItem* previous) {
  if (current != nullptr) {
    ui->name->setText(current->text());
    RuleInfo info = current->data(Qt::UserRole).value<RuleInfo>();
    ui->program->setText(info.program);
    ui->windowNameRegex->setText(info.window_name_regex);
    icon_->setIcon(info.icon);
  }
}

void IconOverrideRulesDialog::addRule() {
  addRule(i18n("New rule"), "", "", "xorg");
  resetRulePresets();
}

void IconOverrideRulesDialog::removeSelectedRule() {
  QListWidgetItem* item = rules_->takeItem(rules_->currentRow());
  if (item != nullptr) {
    delete item;
  }
}

void IconOverrideRulesDialog::removeAllRules() {
  rules_->clear();
  clearItemDetails();
}

void IconOverrideRulesDialog::updateSelectedRule() {
  QListWidgetItem* item = rules_->currentItem();
  if (item != nullptr) {
    item->setText(ui->name->text());
    item->setIcon(getListItemIcon(icon_->icon()));
    item->setData(Qt::UserRole, QVariant::fromValue(
        RuleInfo(ui->program->text(), ui->windowNameRegex->text(), icon_->icon())));
  }
}

void IconOverrideRulesDialog::resetRulePresets() {
  // TODO: consider adding presets/templates?
  //ui->rulePresets->setCurrentIndex(0);
}

void IconOverrideRulesDialog::loadData() {
  rules_->clear();
  for (const auto& item : model_->iconOverrideRules()) {
    QPixmap icon = KIconLoader::global()->loadIcon(
        item.icon, KIconLoader::NoGroup, kListIconSize);
    QListWidgetItem* listItem = new QListWidgetItem(icon, item.name);
    listItem->setData(Qt::UserRole, QVariant::fromValue(
        RuleInfo(item.program, item.window_name_regex, item.icon)));
    rules_->addItem(listItem);
  }
  rules_->setCurrentRow(0);
}

void IconOverrideRulesDialog::saveData() {
  const int ruleCount = rules_->count();
  std::vector<IconOverrideRule> rules;
  rules.reserve(ruleCount);
  for (int i = 0; i < ruleCount; ++i) {
    auto* listItem = rules_->item(i);
    auto info = listItem->data(Qt::UserRole).value<RuleInfo>();
    rules.push_back(IconOverrideRule(
        listItem->text(), info.program, info.window_name_regex, info.icon));
  }
  model_->setIconOverrideRules(rules);
  model_->saveIconOverrideRules();
}

void IconOverrideRulesDialog::clearItemDetails() {
  ui->name->clear();
  ui->program->clear();
  ui->windowNameRegex->clear();
}

}  // namespace ksmoothdock
