#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnetsfu.h"
#include "dialogcreateroom.h"
#include "dialogparticipant.h"
#include "dialogseek.h"
#include "dialoglimit.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  controller_(new Controller)
{
  ui->setupUi(this);
  connect(controller_, &Controller::showLog, this, &MainWindow::on_ShowLog);
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
  DialogConnetSfu conn(this);
  conn.exec();
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
  DialogSeek seek(this);
  seek.exec();
}

void MainWindow::on_actionLimit_triggered()
{
  DialogLimit limit(this);
  limit.exec();
}

void MainWindow::on_actionLeaveRoom_triggered()
{
  controller_->leaveRoom();
}

void MainWindow::on_actionStreamCamera_triggered()
{
  controller_->publishCamera();
}

void MainWindow::on_actionStreamDesktop_triggered()
{
  controller_->publishDesktop();
}

void MainWindow::on_actionLastNone_triggered()
{
  controller_->lastN(0);
}

void MainWindow::on_actionLastOne_triggered()
{
  controller_->lastN(1);

}

void MainWindow::on_actionLastTwo_triggered()
{
  controller_->lastN(2);

}

void MainWindow::on_actionLastFour_triggered()
{
  controller_->lastN(4);

}

void MainWindow::on_actionLastAll_triggered()
{
  controller_->lastN(-1);

}

void MainWindow::on_actionClearLog_triggered()
{
  ui->logWindow->clear();
}

void MainWindow::on_ShowLog(const QString &log)
{
  ui->logWindow->appendPlainText(log);
}

void MainWindow::on_actionSaveLog_triggered()
{
    auto save = QFileDialog::getSaveFileName(this, tr("Save Log"), QDir::currentPath().append("/log.txt"), tr("txt"));
    QFile f(save);
    if (f.open(QIODevice::WriteOnly)) {
      QTextStream s(&f);
      s << ui->logWindow->toPlainText();
      f.flush();
      f.close();
    }

    QDesktopServices::openUrl(QUrl("file://" + save));
}
