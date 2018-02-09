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

#ifndef KSMOOTHDOCK_CONFIG_DIALOG_H_
#define KSMOOTHDOCK_CONFIG_DIALOG_H_

#include <QAbstractButton>
#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>

#include <KColorButton>

namespace ksmoothdock {

class KSmoothDock;

// Global appearance configurations.
class ConfigDialog : public QDialog {
  Q_OBJECT

 public:
  ConfigDialog(KSmoothDock* parent);
  ~ConfigDialog() {}

 public slots:
  void buttonClicked(QAbstractButton* button);

 private:
  KSmoothDock* parent_;

  QLabel *minSizeLabel_;
  QSpinBox *minSize_;
  QLabel *maxSizeLabel_;
  QSpinBox *maxSize_;

  QLabel *backgroundAlphaLabel_;
  QDoubleSpinBox *backgroundAlpha_;
  QLabel *backgroundColorLabel_;
  KColorButton *backgroundColor_;

  QCheckBox *showBorder_;
  QLabel *borderColorLabel_;
  KColorButton *borderColor_;

  QLabel *tooltipFontSizeLabel_;
  QSpinBox *tooltipFontSize_;

  QDialogButtonBox *buttonBox_;

  friend class KSmoothDock;
  friend class ConfigDialogTest;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_CONFIG_DIALOG_H_
