#include "desktop_selector.h"

#include <cstdlib>
#include <memory>

#include <QApplication>
#include <QDesktopWidget>
#include <QTemporaryFile>
#include <QTest>

namespace ksmoothdock {

class DesktopSelectorTest: public QObject {
  Q_OBJECT

 private slots:
  void init() {
    configFile_.reset(new QTemporaryFile);
    QVERIFY(configFile_->open());
    config_.reset(new KConfig(configFile_->fileName(), KConfig::SimpleConfig));
    desktopSelector_.reset(new DesktopSelector(
        nullptr, Qt::Horizontal, kMinSize, kMaxSize, 1, config_.get()));
  }

  // Tests that the icon is scaled to screen's width/height ratio if needed.
  void setIconScaled();

 private:
  static const int kMinSize = 64;
  static const int kMaxSize = 64;

  std::unique_ptr<DesktopSelector> desktopSelector_;
  std::unique_ptr<QTemporaryFile> configFile_;
  std::unique_ptr<KConfig> config_;
};
const int DesktopSelectorTest::kMinSize;
const int DesktopSelectorTest::kMaxSize;

void DesktopSelectorTest::setIconScaled() {
  desktopSelector_->setIconScaled(QPixmap(100, 100));
  QCOMPARE(desktopSelector_->getIcon(kMinSize).height(), kMinSize);
  const int desktopWidth = QApplication::desktop()->screenGeometry().width();
  const int desktopHeight = QApplication::desktop()->screenGeometry().height();
  // Gives room to rounding difference.
  QVERIFY(abs(desktopSelector_->getIcon(kMinSize).width() -
              desktopWidth * kMinSize / desktopHeight) <= 1);
}

}  // namespace ksmoothdock

QTEST_MAIN(ksmoothdock::DesktopSelectorTest)
#include "desktop_selector_test.moc"
