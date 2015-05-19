#ifndef KSMOOTHDOCK_PARABOLIC_ZOOM_VIEW_H_
#define KSMOOTHDOCK_PARABOLIC_ZOOM_VIEW_H_

#include "dockview.h"

#include <QMainWindow>

namespace ksmoothdock {

class ParabolicZoomView : public QMainWindow, public DockView {
  Q_OBJECT

 public:
  ParabolicZoomView();
  virtual ~ParabolicZoomView();

  virtual void resize(int w, int h);
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_PARABOLIC_ZOOM_VIEW_H_
