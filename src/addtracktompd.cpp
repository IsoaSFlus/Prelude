#include <QtCore>
#include <QtNetwork>
#include "addtracktompd.h"
#include "mpd/mpd_client.h"
#include "json/CJsonObject.hpp"

AddTrackToMPD::AddTrackToMPD(QString album_id, QString host, QString ip, QObject *parent)
    : QObject(parent)
    , album_id(album_id.mid(10))
{
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &AddTrackToMPD::httpFinished);
    MPDClient::getInstance().config(host, ip.toUInt());
    QTimer::singleShot(10, this, &AddTrackToMPD::run);
}

void AddTrackToMPD::httpFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Qobuz add album: " << reply->error();
        reply->deleteLater();
        QTimer::singleShot(3000, []() {
            QCoreApplication::quit();
        });
        return;
    }
    QStringList tracks;
    neb::CJsonObject json(reply->readAll().toStdString());
    uint i = 0;
    do {
        tracks.append(QStringLiteral("qobuz://track/").append(QString::fromStdString(json["tracks"]["items"][i]["id"].ToString())));
    } while (!json["tracks"]["items"][++i].IsEmpty());
    MPDClient::getInstance().addTracks(tracks);
    reply->deleteLater();
    QTimer::singleShot(3000, []() {
        QCoreApplication::quit();
    });
}

void AddTrackToMPD::run()
{
    QUrl url("https://www.qobuz.com/api.json/0.2/album/get");
    QUrlQuery query;
    query.addQueryItem("album_id", album_id);
    query.addQueryItem("app_id", "950096963");
    url.setQuery(query);
    QNetworkRequest qnr(url);
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));
    qnam->get(qnr);
}
