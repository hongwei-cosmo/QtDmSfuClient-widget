#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  // NOTE: Enable it if want to start in Max mode
  // w.setWindowState(Qt::WindowMaximized);
  w.show();

  return a.exec();
}
