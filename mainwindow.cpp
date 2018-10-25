#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnetsfu.h"
#include "dialogcreateroom.h"
#include "dialogparticipant.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  controller_(new Controller)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  qDebug("[%s]", __func__);
  delete ui;
  delete controller_;
}

Controller *MainWindow::controller() const
{
  return controller_;
}

void MainWindow::on_actionConnect_triggered()
{
  DialogConnetSfu connect(this);
  connect.exec();
}

void MainWindow::on_actionDisconnect_triggered()
{
  controller_->disconnectSfu();
}

void MainWindow::on_actionExit_triggered()
{
  this->close();
}

void MainWindow::on_actionAbout_triggered()
{
  QMessageBox::about(this, tr("Darkmatter SFU Client"),
                     tr("The <b>Client</b> is used to control Darkmatter "
                        "SFU Server."));
}

void MainWindow::on_actionCreateRoom_triggered()
{
  DialogCreateRoom create(this);
  create.exec();
}

void MainWindow::on_actionCreateAuditRoom_triggered()
{
  DialogCreateRoom create(this, true);
  create.exec();
}

void MainWindow::on_actionDestroyRoom_triggered()
{
    controller_->destroyRoom();
}

void MainWindow::on_actionJoinRoom_triggered()
{
    DialogParticipant join(this);
    join.exec();
}

void MainWindow::on_actionSeek_triggered()
{

}

void MainWindow::on_actionLimit_triggered()
{

}

void MainWindow::on_actionLeaveRoom_triggered()
{
    controller_->leaveRoom();
}

void MainWindow::on_actionStreamCamera_triggered()
{

}

void MainWindow::on_actionStreamDesktop_triggered()
{

}

void MainWindow::on_actionLastNone_triggered()
{

}

void MainWindow::on_actionLastOne_triggered()
{

}

void MainWindow::on_actionLastTwo_triggered()
{

}

void MainWindow::on_actionLastFour_triggered()
{

}

void MainWindow::on_actionLastAll_triggered()
{

}

void MainWindow::on_actionClearLog_triggered()
{

}
