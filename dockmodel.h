#ifndef KSMOOTHDOCK_DOCK_MODEL_H_
#define KSMOOTHDOCK_DOCK_MODEL_H_

#include <vector>

#include "dockitem.h"

namespace ksmoothdock {

class DockModel {
 public:
  DockModel();
  ~DockModel() {}

 private:
  void LoadLaunchers();

  std::vector<DockItem> items_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DOCK_MODEL_H_
