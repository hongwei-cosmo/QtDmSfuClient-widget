#ifndef DIALOGCREATEROOM_H
#define DIALOGCREATEROOM_H

#include <QDialog>

namespace Ui {
class DialogCreateRoom;
}

class DialogCreateRoom : public QDialog
{
  Q_OBJECT

public:
  explicit DialogCreateRoom(QWidget *parent = nullptr);
  ~DialogCreateRoom();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogCreateRoom *ui;
};

#endif // DIALOGCREATEROOM_H
