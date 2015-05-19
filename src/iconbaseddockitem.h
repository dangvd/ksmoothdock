#ifndef KSMOOTHDOCK_ICON_BASED_DOCK_ITEM_H_
#define KSMOOTHDOCK_ICON_BASED_DOCK_ITEM_H_

#include <vector>

#include <QPainter>
#include <QPixmap>
#include <QString>
#include <Qt>

#include "dockitem.h"

namespace ksmoothdock {

class IconBasedDockItem : public DockItem {
 public:
  IconBasedDockItem(QString label, QString iconName, int minSize,
    int maxSize, Qt::Orientation orientation);
  ~IconBasedDockItem() {}

  virtual void draw(QPainter* painter, int x, int y, int size);

 protected:
  int minSize_;
  int maxSize_;
  std::vector<QPixmap> icons_;

 private:
  const int kIconLoadSize = 128;

  void generateIcons(const QPixmap& icon, Qt::Orientation orientation);
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_ICON_BASED_DOCK_ITEM_H_
