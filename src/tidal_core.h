#ifndef TIDAL_CORE_H
#define TIDAL_CORE_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <map>

namespace TidalCore {

struct Album
{
    uint aid;
    QString title;
    QByteArray cover;
};

class Tidal : public QObject {
    Q_OBJECT

public:
    explicit Tidal(QObject *parent = nullptr);

    void startRequest(const QUrl &requestedUrl);

private:
    QNetworkAccessManager qnam;
    QByteArray response;
    std::map<uint, QString> album;

private slots:
    void httpFinished(QNetworkReply *reply);
//    void httpReadyRead();

};

}


#endif // TIDAL_CORE_H
