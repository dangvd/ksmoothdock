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

#include "wallpaper_settings_dialog.h"
#include "ui_wallpaper_settings_dialog.h"

#include <algorithm>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QPixmap>

#include <KWindowSystem>

#include <utils/wallpaper_utils.h>

namespace ksmoothdock {

WallpaperSettingsDialog::WallpaperSettingsDialog(QWidget* parent,
                                                 MultiDockModel* model)
    : QDialog(parent),
      ui(new Ui::WallpaperSettingsDialog),
      model_(model) {
  ui->setupUi(this);

  // Populate desktop list.

  for (int desktop = 1; desktop <= KWindowSystem::numberOfDesktops();
       ++desktop) {
    ui->desktop->addItem(QString::number(desktop));
  }

  // Populate screen list.

  const int screenCount = QApplication::desktop()->screenCount();
  for (int i = 1; i <= screenCount; ++i) {
    ui->screen->addItem(QString("Screen ") + QString::number(i));
  }
  ui->screen->setCurrentIndex(0);
  updatePreviewSize();

  // Adjust the UI for single/multi-screen.

  const bool isSingleScreen = (screenCount == 1);
  if (isSingleScreen) {
    ui->screenLabel->setVisible(false);
    ui->screen->setVisible(false);
  }

  connect(ui->desktop, SIGNAL(currentIndexChanged(int)),
          this, SLOT(onDesktopChanged(int)));
  connect(ui->browse, SIGNAL(clicked()), this, SLOT(browseWallpaper()));
  connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
      this, SLOT(buttonClicked(QAbstractButton*)));
}

WallpaperSettingsDialog::~WallpaperSettingsDialog() {
  delete ui;
}

void WallpaperSettingsDialog::setDesktop(int desktop) {
  ui->desktop->setCurrentIndex(desktop - 1);
  loadData();
}

void WallpaperSettingsDialog::accept() {
  QDialog::accept();
  saveData();
}

void WallpaperSettingsDialog::buttonClicked(QAbstractButton *button) {
  auto role = ui->buttonBox->buttonRole(button);
  if (role == QDialogButtonBox::ApplyRole) {
    saveData();
  }
}

void WallpaperSettingsDialog::browseWallpaper() {
  const QString& wallpaper = QFileDialog::getOpenFileName(
        nullptr,
        i18n("Select Wallpaper Image"),
        QDir::homePath(),
        i18n("Image Files (*.png *.jpg *.bmp)"));
  if (wallpaper.isEmpty()) {
    return;
  }

  wallpaper_ = wallpaper;
  ui->preview->setPixmap(QPixmap(wallpaper_));
}

void WallpaperSettingsDialog::updatePreviewSize() {
  const auto screenGeometry = QApplication::desktop()->screenGeometry(screen());
  const auto scale = std::min(
      static_cast<float>(kMaxPreviewWidth) / screenGeometry.width(),
      static_cast<float>(kMaxPreviewHeight) / screenGeometry.height());
  const auto w = static_cast<int>(scale * screenGeometry.width());
  const auto h = static_cast<int>(scale * screenGeometry.height());
  const auto x = (ui->previewHolder->width() - w) / 2;
  const auto y = (ui->previewHolder->height() - h) / 2;
  ui->preview->setGeometry(x, y, w, h);
}

void WallpaperSettingsDialog::onDesktopChanged(int desktop) {
  loadData();
}

int WallpaperSettingsDialog::screen() const {
  return ui->screen->currentIndex();
}

int WallpaperSettingsDialog::desktop() const {
  return ui->desktop->currentIndex() + 1;
}

void WallpaperSettingsDialog::loadData() {
  wallpaper_ = model_->wallpaper(desktop());
  ui->preview->setPixmap(QPixmap(
      !wallpaper_.isEmpty() ? wallpaper_ : loadPlasmaWallpaper()));
}

void WallpaperSettingsDialog::saveData() {
  if (!wallpaper_.isEmpty()) {
    model_->setWallpaper(desktop(), wallpaper_);
    model_->saveAppearanceConfig();
  }
}

}  // namespace ksmoothdock
