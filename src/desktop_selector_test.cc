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

#include <cstdlib>
#include <memory>

#include <QApplication>
#include <QDesktopWidget>
#include <QTemporaryFile>
#include <QTest>

namespace ksmoothdock {

class DesktopSelectorTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configFile_.reset(new QTemporaryFile);
    QVERIFY(configFile_->open());
    config_.reset(new KConfig(configFile_->fileName(), KConfig::SimpleConfig));
    desktopSelector_.reset(new DesktopSelector(
        nullptr, Qt::Horizontal, kMinSize, kMaxSize, 1, config_.get()));
  }

  // Tests that the icon is scaled to screen's width/height ratio if needed.
  void setIconScaled();

 private:
  static const int kMinSize = 64;
  static const int kMaxSize = 64;

  std::unique_ptr<DesktopSelector> desktopSelector_;
  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<QTemporaryFile> appearanceConfigFile_;
  std::unique_ptr<KConfig> config_;
};
const int DesktopSelectorTest::kMinSize;
const int DesktopSelectorTest::kMaxSize;

void DesktopSelectorTest::setIconScaled() {
  desktopSelector_->setIconScaled(QPixmap(100, 100));
  QCOMPARE(desktopSelector_->getIcon(kMinSize).height(), kMinSize);
  const int desktopWidth = QApplication::desktop()->screenGeometry().width();
  const int desktopHeight = QApplication::desktop()->screenGeometry().height();
  // Gives room to rounding difference.
  QVERIFY(abs(desktopSelector_->getIcon(kMinSize).width() -
              desktopWidth * kMinSize / desktopHeight) <= 1);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::DesktopSelectorTest)
#include "desktop_selector_test.moc"
