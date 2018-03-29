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

#include "desktop_selector.h"

#include <cmath>
#include <memory>

#include <QApplication>
#include <QDesktopWidget>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>

#include "multi_dock_view.h"

namespace ksmoothdock {

constexpr int kDockId = 1;
constexpr int kDesktop = 1;
constexpr int kScreen = 0;
constexpr int kMinSize = 64;
constexpr int kMaxSize = 64;

class DesktopSelectorTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    QTemporaryDir configDir;
    model_ = std::make_unique<MultiDockModel>(configDir.path());
    model_->addDock();
    view_ = std::make_unique<MultiDockView>(model_.get());
    dock_ = std::make_unique<DockPanel>(view_.get(), model_.get(), kDockId);
  }

  // Tests that the icon is scaled to screen's width/height ratio if needed.
  void setIconScaled();

 private:
  std::unique_ptr<MultiDockModel> model_;
  std::unique_ptr<MultiDockView> view_;
  std::unique_ptr<DockPanel> dock_;
};

void DesktopSelectorTest::setIconScaled() {
  QTemporaryFile iconFile;
  QVERIFY(iconFile.open());
  QPixmap icon(100, 100);
  icon.save(iconFile.fileName(), "PNG");
  model_->setWallpaper(kDesktop, kScreen, iconFile.fileName());

  DesktopSelector desktopSelector(dock_.get(), model_.get(), Qt::Horizontal,
                                  kMinSize, kMaxSize, kDesktop, kScreen);

  QCOMPARE(desktopSelector.getIcon(kMinSize).height(), kMinSize);
  const int desktopWidth = QApplication::desktop()->screenGeometry().width();
  const int desktopHeight = QApplication::desktop()->screenGeometry().height();
  // Gives room to rounding difference.
  QVERIFY(std::abs(desktopSelector.getIcon(kMinSize).width() -
                   desktopWidth * kMinSize / desktopHeight) <= 1);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::DesktopSelectorTest)
#include "desktop_selector_test.moc"
