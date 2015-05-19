#ifndef KSMOOTHDOCK_LAUNCHER_H_
#define KSMOOTHDOCK_LAUNCHER_H_

#include <QString>
#include <Qt>

#include "iconbaseddockitem.h"

namespace ksmoothdock {

class Launcher : public IconBasedDockItem {
 public:
  Launcher(QString label, QString iconName, int minSize, int maxSize,
    Qt::Orientation orientation, QString command);

 private:
  QString command_;
};

}

#endif  // KSMOOTHDOCK_LAUNCHER_H_
