#include "frame_widget.h"

void FrameWidget::drawImage(const QImage &image)
{
  const QPixmap &pixmap = QPixmap::fromImage(image);
//  if ((label->width() != image.width()) || (label->height() != image.height())) {
//    label->resize(image.width(), image.height());
//  }
  setPixmap(pixmap);
}
