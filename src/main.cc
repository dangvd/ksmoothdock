#include <QApplication>

#include "ksmoothdock.h"

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  ksmoothdock::KSmoothDock dock;
  dock.init();
  dock.show();
  return app.exec();
}
