#ifndef KSMOOTHDOCK_DOCK_ITEM_H_
#define KSMOOTHDOCK_DOCK_ITEM_H_

#include <QString>

namespace ksmoothdock {

class DockItem {
 public:
  DockItem(QString label) : label_(label) {}
  ~DockItem() {}

  virtual void draw(QPainter* painter, int x, int y, int size) = 0;
 protected:
  QString label_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_ITEM_H_
