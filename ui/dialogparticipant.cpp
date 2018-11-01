#include "dialogparticipant.h"
#include "mainwindow.h"
#include "ui_dialogparticipant.h"

DialogParticipant::DialogParticipant(QWidget *parent, bool join) :
  QDialog(parent),
  ui(new Ui::DialogParticipant),
  join_(join)
{
  ui->setupUi(this);
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  ui->lineEdit_Room->setText(QString::fromStdString(controller->getRoomId()));
  ui->lineEdit_PIN->setText(QString::fromStdString(controller->getRoomAccessPin()));
  ui->label_PIN->setVisible(join_);
  ui->lineEdit_PIN->setVisible(join_);
}

DialogParticipant::~DialogParticipant()
{
  delete ui;
}

void DialogParticipant::on_buttonBox_accepted()
{
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  controller->setRoomId(ui->lineEdit_Room->text().toStdString());
  controller->setRoomAccessPin(ui->lineEdit_PIN->text().toStdString());
  controller->joinRoom();
}
