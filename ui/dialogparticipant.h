#ifndef DIALOGPARTICIPANT_H
#define DIALOGPARTICIPANT_H

#include <QDialog>

namespace Ui {
class DialogParticipant;
}

class DialogParticipant : public QDialog
{
  Q_OBJECT

public:
  explicit DialogParticipant(QWidget *parent = nullptr, bool join = true);
  ~DialogParticipant();

private Q_SLOTS:
  void on_buttonBox_accepted();

private:
  Ui::DialogParticipant *ui;
  bool join_ = true;
};

#endif // DIALOGPARTICIPANT_H
