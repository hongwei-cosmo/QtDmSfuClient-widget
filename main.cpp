#include "mainwindow.h"
#include "controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  Controller c;
  c.connectSfu("wss://192.168.2.2:9443", "client3333");
  return a.exec();
}
