#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lib/controller.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  Controller* controller() const;

private Q_SLOTS:
  void on_actionConnect_triggered();

  void on_actionDisconnect_triggered();

  void on_actionExit_triggered();

  void on_actionAbout_triggered();

  void on_actionCreateRoom_triggered();

  void on_actionCreateAuditRoom_triggered();

  void on_actionDestroyRoom_triggered();

  void on_actionJoinRoom_triggered();

  void on_actionSeek_triggered();

  void on_actionLimit_triggered();

  void on_actionLeaveRoom_triggered();

  void on_actionStreamCamera_triggered();

  void on_actionStreamDesktop_triggered();

  void on_actionLastNone_triggered();

  void on_actionLastOne_triggered();

  void on_actionLastTwo_triggered();

  void on_actionLastFour_triggered();

  void on_actionLastAll_triggered();

  void on_actionClearLog_triggered();

  void on_actionSaveLog_triggered();

private:
  Ui::MainWindow *ui = nullptr;
  Controller *controller_ = nullptr;
};

#endif // MAINWINDOW_H
