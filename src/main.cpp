#include <QtCore>
#include "tidal_core.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TidalCore::Tidal t;
    a.exec();
    return 0;
}
