#ifndef DIALOGSEEK_H
#define DIALOGSEEK_H

#include <QDialog>

namespace Ui {
class DialogSeek;
}

class DialogSeek : public QDialog
{
  Q_OBJECT

public:
  explicit DialogSeek(QWidget *parent = nullptr);
  ~DialogSeek();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogSeek *ui;
};

#endif // DIALOGSEEK_H
