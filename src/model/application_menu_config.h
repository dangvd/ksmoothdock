#ifndef KSMOOTHDOCK_APPLICATION_MENU_CONFIG_H_
#define KSMOOTHDOCK_APPLICATION_MENU_CONFIG_H_

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <QDir>
#include <QEvent>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QStringList>

#include <utils/command_utils.h>

namespace ksmoothdock {

// An application entry in the application menu.
struct ApplicationEntry {
  // Name e.g. 'Chrome'.
  QString name;

  // Generic name e.g. 'Web Brower'.
  QString genericName;

  // Icon name e.g. 'chrome'.
  QString icon;

  // Command to execute e.g. '/usr/bin/google-chrome-stable'.
  QString command;

  // The task command, to compare with KWindowInfo.windowClassName, e.g. 'google-chrome'
  QString taskCommand;

  // The path to the desktop file e.g. '/usr/share/applications/chrome.desktop'
  QString desktopFile;

  ApplicationEntry(const QString& name2, const QString& genericName2,
                   const QString& icon2, const QString& command2,
                   const QString& desktopFile2)
      : name(name2), genericName(genericName2), icon(icon2), command(command2),
        taskCommand(getTaskCommand(command)), desktopFile(desktopFile2) {}
};

bool operator<(const ApplicationEntry &e1, const ApplicationEntry &e2);

// A category in the application menu.
struct Category {
  // Name for the category e.g. 'Development' or 'Utility'. See:
  // https://specifications.freedesktop.org/menu-spec/latest/apa.html
  QString name;

  // Display name for the category e.g. 'Utilities'.
  QString displayName;

  // Icon name for the category e.g. 'applications-internet'.
  QString icon;

  // Application entries for this category.
  std::list<ApplicationEntry> entries;

  Category(const QString& name2, const QString& displayName2,
           const QString& icon2)
      : name(name2), displayName(displayName2), icon(icon2) {}

  Category(const QString& name2, const QString& displayName2,
           const QString& icon2, std::list<ApplicationEntry> entries2)
      : name(name2), displayName(displayName2), icon(icon2), entries(entries2) {
  }
};

class ApplicationMenuConfig : public QObject {
  Q_OBJECT

 public:
  ApplicationMenuConfig(const QStringList& entryDirs = {
      "/usr/share/applications",
      "/usr/share/applications/kde4",
      QDir::homePath() + "/.local/share/applications"});

  ~ApplicationMenuConfig() = default;

  static const std::vector<Category> kSessionSystemCategories;
  static const ApplicationEntry kSearchEntry;

  const std::vector<Category>& categories() const { return categories_; }

  const ApplicationEntry* findApplication(const std::string& command) const;
  const ApplicationEntry* findApplication(const QString& command) const {
    return findApplication(command.toStdString());
  }

 signals:
  void configChanged();

 public slots:
  void reload();

 private:
  // Initializes application categories.
  void initCategories();

  // Loads application entries from entryDir.
  bool loadEntries();

  // Loads an application entry from the .desktop file.
  bool loadEntry(const QString& file);

  // The directories that contains the list of all application entries as
  // desktop files, e.g. /usr/share/applications
  const QStringList entryDirs_;

  // Application entries, organized by categories.
  std::vector<Category> categories_;
  // Map from category names to category indices in the above vector,
  // to make loading entries faster.
  std::unordered_map<std::string, int> categoryMap_;
  // Map from commands to application entries for fast look-up.
  std::unordered_map<std::string, const ApplicationEntry*> entries_;

  QFileSystemWatcher fileWatcher_;

  friend class ApplicationMenuConfigTest;
};

}

#endif  // KSMOOTHDOCK_APPLICATION_MENU_CONFIG_H_
