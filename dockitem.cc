#include "dockitem.h"

#include <KIconLoader>

#include <QImage>

namespace ksmoothdock {

DockItem::DockItem(QString label, int min_size, int max_size, QString icon_name)
    : label_(label), min_size_(min_size), max_size_(max_size),
      icons_(max_size - min_size + 1) {
  KIconLoader* loader = KIconLoader::global();
  QPixmap icon = loader->loadIcon(icon_name, KIconLoader::NoGroup,
      kIconLoadSize);
  GenerateIcons(icon);
}

void DockItem::GenerateIcons(const QPixmap& icon) {
  int original_width = icon.width();
  int original_height = icon.height();

  QImage image = icon.toImage(); // Convert to QImage for fast scaling.
  for (int h = min_size_; h <= max_size_; ++h) {
    icons_[h - min_size_] = QPixmap::fromImage(
        image.scaledToHeight(h, Qt::SmoothTransformation));
  }
}

}  // namespace ksmoothdock
