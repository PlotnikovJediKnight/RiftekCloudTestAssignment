#include "mainwindow.h"

#include <QApplication>
#include "simpleViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(800, 565);
    w.show();
    return a.exec();
}
