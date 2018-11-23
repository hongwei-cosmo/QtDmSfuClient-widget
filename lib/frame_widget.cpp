#include "frame_widget.h"

void FrameWidget::drawImage(const QImage &image)
{
  const QPixmap &pixmap = QPixmap::fromImage(image);
  setPixmap(pixmap.scaled(width(), height(), Qt::KeepAspectRatio));
}
