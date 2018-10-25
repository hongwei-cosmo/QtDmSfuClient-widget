#include "dialogcreateroom.h"
#include "mainwindow.h"
#include "ui_dialogcreateroom.h"

DialogCreateRoom::DialogCreateRoom(QWidget *parent, bool audit) :
  QDialog(parent),
  ui(new Ui::DialogCreateRoom),
  audit_(audit)
{
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  ui->setupUi(this);
  ui->lineEdit_PIN->setText(QString::fromStdString(controller->getRoomAccessPin()));
  ui->label_RecordingID->setVisible(audit_);
  ui->lineEdit_RecordingID->setVisible(audit_);
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

  controller->setRoomAccessPin(ui->lineEdit_PIN->text().toStdString());
  if (audit_) {
    controller->createAuditRoom(ui->lineEdit_RecordingID->text().toStdString());
  } else {
    controller->createRoom();
  }
}
