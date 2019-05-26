#ifndef TIDAL_CORE_H
#define TIDAL_CORE_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <map>

namespace TidalCore {

struct Album
{
    uint aid;
    std::string title;
    std::string cover;
    QStringList date;
};

class Tidal : public QObject {
    Q_OBJECT

public:
    explicit Tidal(QObject *parent = nullptr);

    void queryAlbum();
    void startRequest(const QUrl &requestedUrl);
    void printResult();

private:
    QNetworkAccessManager qnam;
    QByteArray response;
    std::map<uint, TidalCore::Album> albums_map;
    std::vector<Album> detail_albums;
    uint finished_count = 0;
    uint request_count = 0;

private slots:
    void httpFinished(QNetworkReply *reply);
//    void httpReadyRead();

};

}


#endif // TIDAL_CORE_H
