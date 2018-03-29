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

#include "application_menu.h"

#include <memory>

#include <QTemporaryDir>
#include <QTest>

#include "multi_dock_view.h"

namespace ksmoothdock {

constexpr int kDockId = 1;
constexpr int kMinSize = 64;
constexpr int kMaxSize = 64;
constexpr int kNumCategories = 11;

class ApplicationMenuTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    QTemporaryDir configDir;
    model_ = std::make_unique<MultiDockModel>(configDir.path());
    model_->addDock();
    view_ = std::make_unique<MultiDockView>(model_.get());
    dock_ = std::make_unique<DockPanel>(view_.get(), model_.get(), kDockId);
  }

  void loadEntries_singleDir();
  void loadEntries_multipleDirs();

 private:

  void writeEntry(const QString& filename, const ApplicationEntry& entry,
                  const QString& categories,
                  const std::unordered_map<std::string, std::string>& extraKVs
                      = {}) {
    KConfig config(filename, KConfig::SimpleConfig);
    KConfigGroup group(&config, "Desktop Entry");
    group.writeEntry("Name", entry.name);
    group.writeEntry("GenericName", entry.genericName);
    group.writeEntry("Icon", entry.icon);
    group.writeEntry("Exec", entry.command);
    group.writeEntry("Categories", categories);
    for (const auto& kv : extraKVs) {
      group.writeEntry(QString::fromStdString(kv.first),
                       QString::fromStdString(kv.second));
    }
    config.sync();
  }

  std::unique_ptr<MultiDockModel> model_;
  std::unique_ptr<MultiDockView> view_;
  std::unique_ptr<DockPanel> dock_;
};

void ApplicationMenuTest::loadEntries_singleDir() {
  QTemporaryDir entryDir;
  QVERIFY(entryDir.isValid());
  writeEntry(entryDir.path() + "/1.desktop",
             {"Chrome", "Web Browser", "chrome", "chrome", ""},
             "Network");

  ApplicationMenu applicationMenu(dock_.get(), model_.get(), Qt::Horizontal,
                                  kMinSize, kMaxSize, { entryDir.path() });

  QCOMPARE(static_cast<int>(applicationMenu.categories_.size()),
           kNumCategories);
  for (const auto& category : applicationMenu.categories_) {
    if (category.name == "Network") {
      QCOMPARE(static_cast<int>(category.entries.size()), 1);
    } else {
      QCOMPARE(static_cast<int>(category.entries.size()), 0);
    }
  }
}

void ApplicationMenuTest::loadEntries_multipleDirs() {
  QTemporaryDir entryDir1;
  QVERIFY(entryDir1.isValid());
  writeEntry(entryDir1.path() + "/1.desktop",
             {"Chrome", "Web Browser", "chrome", "chrome", ""},
             "Network");
  writeEntry(entryDir1.path() + "/2.desktop",
             {"KMail", "Email Client", "kmail", "kmail", ""},
             "Qt;KDE;Network");
  writeEntry(entryDir1.path() + "/3.desktop",
             {"Xfce Settings", "", "xfce-settings", "xfce-settings", ""},
             "Settings",
             {{"OnlyShowIn", "Xfce"}});

  // Empty dir
  QTemporaryDir entryDir2;
  QVERIFY(entryDir2.isValid());

  QTemporaryDir entryDir3;
  QVERIFY(entryDir3.isValid());
  writeEntry(entryDir3.path() + "/1.desktop",
             {"KDE Settings", "", "systemsettings5", "systemsettings5", ""},
             "Settings",
             {{"OnlyShowIn", "KDE"}});
  writeEntry(entryDir3.path() + "/2.desktop",
             {"Gnome Settings", "", "gnome-settings", "gnome-settings", ""},
             "Settings",
             {{"NotShowIn", "KDE"}});
  writeEntry(entryDir3.path() + "/3.desktop",
             {"Chrome - HTML", "Web Browser", "chrome", "chrome", ""},
             "Network",
             {{"NoDisplay", "true"}});
  writeEntry(entryDir3.path() + "/4.desktop",
             {"Chrome - Old", "Web Browser", "chrome", "chrome", ""},
             "Network",
             {{"Hidden", "true"}});

  ApplicationMenu applicationMenu(
      dock_.get(), model_.get(), Qt::Horizontal, kMinSize, kMaxSize,
      { entryDir1.path(), entryDir1.path() + "/dir-not-exist", entryDir2.path(),
        entryDir3.path() });

  QCOMPARE(static_cast<int>(applicationMenu.categories_.size()),
           kNumCategories);
  for (const auto& category : applicationMenu.categories_) {
    if (category.name == "Network") {
      QCOMPARE(static_cast<int>(category.entries.size()), 2);
    } else if (category.name == "Settings") {
      QCOMPARE(static_cast<int>(category.entries.size()), 1);
    } else {
      QCOMPARE(static_cast<int>(category.entries.size()), 0);
    }
  }
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::ApplicationMenuTest)
#include "application_menu_test.moc"
