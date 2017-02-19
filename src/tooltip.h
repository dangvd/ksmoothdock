/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2017 Viet Dang (dangvd@gmail.com)
 *
 * KSmoothDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KSmoothDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KSmoothDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KSMOOTHDOCK_TOOLTIP_H_
#define KSMOOTHDOCK_TOOLTIP_H_

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QString>
#include <QWidget>

namespace ksmoothdock {

// Tooltip with translucent background.
class Tooltip : public QWidget {
 public:
  Tooltip();

  void setText(const QString& text);
  void setFontFace(const QString& fontFace);
  void setFontItalic(bool val);
  void setFontBold(bool val);
  void setFontSize(int size);
  void setFontColor(const QColor& color);
  void setBackgroundColor(const QColor& color); 

  void updateLayout();

 protected:
  virtual void paintEvent(QPaintEvent* e) override;

 private:
  QString text_;
  QFont font_;
  QColor fontColor_;
  int fontSize_;
  QColor backgroundColor_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_TOOLTIP_H_
