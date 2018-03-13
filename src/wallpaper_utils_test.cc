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

#include "wallpaper_utils.h"

#include <QTemporaryFile>
#include <QTest>
#include <QTextStream>

namespace ksmoothdock {

class WallpaperUtilsTest: public QObject {
  Q_OBJECT

 private slots:
  void testGetPlasmaWallpaper();
};

void WallpaperUtilsTest::testGetPlasmaWallpaper() {
  QTemporaryFile plasmaConfig;
  QVERIFY(plasmaConfig.open());
  QTextStream out(&plasmaConfig);
  out << "[Containments][1][General]" << "\n"
      << "[Containments][1][Wallpaper][org.kde.image][General]" << "\n"
      << "Image=file:///home/wallpaper.png" << "\n";
  plasmaConfig.close();

  QCOMPARE(getPlasmaWallpaper(plasmaConfig.fileName()),
           QString("/home/wallpaper.png"));
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::WallpaperUtilsTest)
#include "wallpaper_utils_test.moc"
