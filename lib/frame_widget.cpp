#include "frame_widget.h"

void FrameWidget::drawImage(const QImage &image)
{
  qDebug("label %d, %d", label->width(), label->height());
  const QPixmap &pixmap = QPixmap::fromImage(image);
//  if ((label->width() != image.width()) || (label->height() != image.height())) {
//    label->resize(image.width(), image.height());
//  }
  label->setPixmap(pixmap);
}

void FrameWidget::resize(const QSize &size)
{
  label->resize(size);
}
