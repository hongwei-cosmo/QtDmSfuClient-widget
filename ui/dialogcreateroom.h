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
  explicit DialogCreateRoom(QWidget *parent = nullptr, bool audit = false);
  ~DialogCreateRoom();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogCreateRoom *ui;
  bool audit_ = false;
};

#endif // DIALOGCREATEROOM_H
