#include "paraboliczoomview.h"

#include <KWindowSystem>
#include <netwm_def.h>

#include <QApplication>
#include <QDesktopWidget>

namespace ksmoothdock {

ParabolicZoomView::ParabolicZoomView() {
  setAttribute(Qt::WA_TranslucentBackground);
  KWindowSystem::setType(winId(), NET::Dock);
  resize(800, 100);
}

ParabolicZoomView::~ParabolicZoomView() {}

void ParabolicZoomView::resize(int w, int h) {
  QWidget::resize(w, h);

  int dw = QApplication::desktop()->width();
  int dh = QApplication::desktop()->height();  
  int x = (dw - w) / 2;
  move(x, dh - h);

  KWindowSystem::setStrut(winId(), 0, 0, 0, h);
}

}  // namespace ksmoothdock

#include "paraboliczoomview.moc"
