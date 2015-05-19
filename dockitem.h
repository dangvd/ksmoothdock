#ifndef KSMOOTHDOCK_DOCK_ITEM_H_
#define KSMOOTHDOCK_DOCK_ITEM_H_

#include <vector>

#include <QPixmap>
#include <QString>

namespace ksmoothdock {

class DockItem {
 public:
  DockItem(QString label, int min_size, int max_size, QString icon_name);
  ~DockItem() {}

 private:
  const int kIconLoadSize = 128;

  void GenerateIcons(const QPixmap& icon);

  QString label_;
  int min_size_;
  int max_size_;
  std::vector<QPixmap> icons_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_ITEM_H_
