#include "frame_widget.h"

FrameWidget::FrameWidget(QWidget *parent) : QLabel(parent)
{
  connect(this, &FrameWidget::paintImage, this, &FrameWidget::onPaintImage);
}

void FrameWidget::onPaintImage(const QImage &image)
{
  const QPixmap &pixmap = QPixmap::fromImage(image);
  setPixmap(pixmap.scaled(width(), height(), Qt::KeepAspectRatio));
}
