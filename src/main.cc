#include <QApplication>

#include "paraboliczoomview.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  ksmoothdock::ParabolicZoomView dock;
  dock.show();
  return app.exec();
}
