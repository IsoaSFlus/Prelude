#include <QtCore>
#include <QApplication>
#include "mainwindow.h"
#include "addtracktompd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QApplication::setApplicationName("Prelude");
//    QApplication::setApplicationVersion(PROJECT_VERSION);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption urlOption(QStringList() << "u" << "url", "Url to add", "string", "none");
    parser.addOption(urlOption);
    QCommandLineOption hostOption(QStringList() << "i" << "host", "Host's ip", "ip", "127.0.0.1");
    parser.addOption(hostOption);
    QCommandLineOption portOption(QStringList() << "p" << "port", "Host's port", "port", "6600");
    parser.addOption(portOption);
    QCommandLineOption debugOption(QStringList() << "d" << "debug", "Show debug info");
    parser.addOption(debugOption);

    parser.process(a);

    std::unique_ptr<MainWindow> w;
    std::unique_ptr<AddTrackToMPD> attm;

    if (parser.value(urlOption) == QStringLiteral("none")) {
        w = std::make_unique<MainWindow>(parser.value(hostOption), parser.value(portOption));
        w->setWindowTitle("Prelude");
        w->show();
    } else {
        attm = std::make_unique<AddTrackToMPD>(parser.value(urlOption), parser.value(hostOption), parser.value(portOption));
    }
    return a.exec();
}
