#include "mainwindow.h"
#include "dialoglimit.h"
#include "ui_dialoglimit.h"

DialogLimit::DialogLimit(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogLimit)
{
  ui->setupUi(this);
}

DialogLimit::~DialogLimit()
{
  delete ui;
}

void DialogLimit::on_buttonBox_accepted()
{
  Controller *controller = ((MainWindow*)parentWidget())->controller();
  controller->seekParticipant(ui->lineEdit_Limit->text().toULongLong());
}
