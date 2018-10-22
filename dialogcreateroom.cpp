#include "dialogcreateroom.h"
#include "mainwindow.h"
#include "ui_dialogcreateroom.h"

DialogCreateRoom::DialogCreateRoom(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogCreateRoom)
{
  ui->setupUi(this);
  ui->lineEdit_PIN->setText(QStringLiteral("pin"));
}

DialogCreateRoom::~DialogCreateRoom()
{
  delete ui;
}

void DialogCreateRoom::on_buttonBox_accepted()
{
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  if (!controller->connectedSfu()) {
    qDebug("[%s] Not yet connected to SFU", __func__);
    return;
  }
  if (ui->lineEdit_PIN->text().isEmpty()) {
    qDebug("[%s] PIN is empty", __func__);
    return;
  }

//  sfu->setRoomAccessPin(ui->lineEdit_PIN->text().toStdString());
  controller->createRoom();
}
