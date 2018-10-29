#ifndef DIALOGLIMIT_H
#define DIALOGLIMIT_H

#include <QDialog>

namespace Ui {
class DialogLimit;
}

class DialogLimit : public QDialog
{
  Q_OBJECT

public:
  explicit DialogLimit(QWidget *parent = nullptr);
  ~DialogLimit();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogLimit *ui;
};

#endif // DIALOGLIMIT_H
