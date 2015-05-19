#include "launcher.h"

namespace ksmoothdock {

Launcher::Launcher(QString label, QString iconName, int minSize,
    int maxSize, Qt::Orientation orientation, QString command)
    : IconBasedDockItem(label, iconName, minSize, maxSize, orientation),
      command_(command) {}

}  // namespace ksmoothdock
