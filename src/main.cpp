#include <QtCore>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Prelude");
    QApplication::setApplicationVersion(PROJECT_VERSION);

    MainWindow w;
    w.setWindowTitle("Prelude");
    w.show();
    return a.exec();
}
