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

#include <QApplication>
#include <QDir>
#include <QIcon>

#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>

#include <model/multi_dock_model.h>
#include <view/multi_dock_view.h>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  KDBusService service(KDBusService::Unique);

  KAboutData about(
      "ksmoothdock",
      "KSmoothDock",
      "5.15 RC1",
      i18n("A cool desktop panel for KDE Plasma 5"),
      KAboutLicense::GPL_V3,
      i18n("Copyright (C) 2019 Viet Dang (dangvd@gmail.com)"),
      "",
      "https://dangvd.github.io/ksmoothdock");
  KAboutData::setApplicationData(about);
  QApplication::setWindowIcon(QIcon::fromTheme("user-desktop"));

  ksmoothdock::MultiDockModel model(QDir::homePath() + "/.ksmoothdock");
  ksmoothdock::MultiDockView view(&model);
  view.show();
  return app.exec();
}
