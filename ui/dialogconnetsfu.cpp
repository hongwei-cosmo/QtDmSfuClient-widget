#include "dialogconnetsfu.h"
#include "ui_dialogconnetsfu.h"
#include "mainwindow.h"
#include <QRandomGenerator>

DialogConnetSfu::DialogConnetSfu(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogConnetSfu)
{
  auto id = QRandomGenerator::global()->generate();
  ui->setupUi(this);
  ui->lineEdit_SfuURL->setText("wss://192.168.1.160:9443");
  ui->lineEdit_ClientID->setText(QStringLiteral("Client%1").arg(id));
}

DialogConnetSfu::~DialogConnetSfu()
{
  delete ui;
}

void DialogConnetSfu::on_buttonBox_accepted()
{
    qDebug("[%s]", __func__);
    ((MainWindow*)parentWidget())->controller()->connectSfu(
          ui->lineEdit_SfuURL->text().toStdString(),
          ui->lineEdit_ClientID->text().toStdString());
}
