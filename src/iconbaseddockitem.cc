#include "iconbaseddockitem.h"

#include <KIconLoader>

#include <QImage>

namespace ksmoothdock {

IconBasedDockItem::IconBasedDockItem(QString label, QString iconName,
    int minSize, int maxSize, Qt::Orientation orientation)
    : DockItem(label), minSize_(minSize), maxSize_(maxSize),
      icons_(maxSize - minSize + 1) {
  QPixmap icon = KIconLoader::global()->loadIcon(iconName,
      KIconLoader::NoGroup, kIconLoadSize);
  generateIcons(icon, orientation);
}

void IconBasedDockItem::draw(QPainter* painter, int x, int y, int size) {
  if (size >= minSize_ && size <= maxSize_) {
    painter->drawPixmap(x, y, icons_[size - minSize_]);
  }
}

void IconBasedDockItem::generateIcons(const QPixmap& icon,
    Qt::Orientation orientation) {
  QImage image = icon.toImage(); // Convert to QImage for fast scaling.
  for (int size = minSize_; size <= maxSize_; ++size) {
    icons_[size - minSize_] = QPixmap::fromImage(
        (orientation == Qt::Horizontal)
            ? image.scaledToHeight(size, Qt::SmoothTransformation)
            : image.scaledToWidth(size, Qt::SmoothTransformation));
  }
}

}  // namespace ksmoothdock
