#ifndef DIALOGCONNETSFU_H
#define DIALOGCONNETSFU_H

#include <QDialog>

namespace Ui {
class DialogConnetSfu;
}

class DialogConnetSfu : public QDialog
{
  Q_OBJECT

public:
  explicit DialogConnetSfu(QWidget *parent = nullptr);
  ~DialogConnetSfu();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogConnetSfu *ui;
};

#endif // DIALOGCONNETSFU_H
