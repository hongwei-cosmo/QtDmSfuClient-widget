#include "dialogseek.h"
#include "mainwindow.h"
#include "ui_dialogseek.h"

DialogSeek::DialogSeek(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogSeek)
{
  ui->setupUi(this);
}

DialogSeek::~DialogSeek()
{
  delete ui;
}

void DialogSeek::on_buttonBox_accepted()
{
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  controller->seekParticipant(ui->lineEdit_Seek->text().toULongLong());
}
