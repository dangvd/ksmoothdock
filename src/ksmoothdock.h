#ifndef KSMOOTHDOCK_KSMOOTHDOCK_H_
#define KSMOOTHDOCK_KSMOOTHDOCK_H_

#include <deque>
#include <memory>

#include <QMainWindow>
#include <QPaintEvent>

#include "dockitem.h"

namespace ksmoothdock {

class KSmoothDock : public QMainWindow {
  Q_OBJECT

 public:
  KSmoothDock();
  virtual ~KSmoothDock();

  void init();
  virtual void resize(int w, int h);

 protected:
  virtual void paintEvent(QPaintEvent* e);

 private:
  const int kDefaultMinSize = 48;
  const int kDefaultMaxSize = 160;

  void loadConfig();
  void loadLaunchers();

  int desktopWidth_;
  int desktopHeight_;
  int minSize_;
  int maxSize_;
  std::deque<std::unique_ptr<DockItem>> items_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_KSMOOTHDOCK_H_
