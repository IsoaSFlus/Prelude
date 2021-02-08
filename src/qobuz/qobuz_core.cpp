#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "qobuz_core.h"
#include "json/CJsonObject.hpp"
#include "album.h"

QobuzCore::Qobuz::Qobuz(QObject *parent)
    : QObject(parent)
{
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Qobuz::httpFinished);
}

void QobuzCore::Qobuz::search(QString keywords)
{
    clear();
    QUrl url("http://www.qobuz.com/api.json/0.2/catalog/search");
    QUrlQuery query;
    query.addQueryItem("query", keywords);
    query.addQueryItem("limit", "500");
    query.addQueryItem("offset", "0");
    query.addQueryItem("app_id", "950096963");
    url.setQuery(query);
    startRequest(url);
}

void QobuzCore::Qobuz::searchByUPC(QString upc, QString title)
{
    clear();
    is_search_upc = true;
    this->upc = upc.toStdString();
    QUrl url("http://www.qobuz.com/api.json/0.2/album/search");
    QUrlQuery query;
    if (title.indexOf(' ', 25) == -1) {
        query.addQueryItem("query", title);
        qDebug() << title;
        upc_search_title = title;
    } else {
        query.addQueryItem("query", title.left(title.indexOf(' ', 25)));
        qDebug() << title.left(title.indexOf(' ', 25));
        upc_search_title = title.left(title.indexOf(' ', 25));
    }
    query.addQueryItem("limit", "500");
    query.addQueryItem("offset", "0");
    query.addQueryItem("app_id", "950096963");
    url.setQuery(query);
    startRequest(url);
}

void QobuzCore::Qobuz::startRequest(const QUrl &requestedUrl)
{
    QNetworkRequest qnr(requestedUrl);
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));

    qnam->get(qnr);
}

void QobuzCore::Qobuz::clear()
{
    qnam->disconnect();
    qnam->deleteLater();
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Qobuz::httpFinished);
    detail_albums.clear();
    upc.clear();
    is_search_upc = false;
    is_search_upc_step2 = false;
    upc_search_title.clear();
    current_page = 0;
}

void QobuzCore::Qobuz::httpFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit searchFinished(0x04);
        clear();
        qDebug() << "Qobuz search error: " << reply->error();
        reply->deleteLater();
        return;
    }
    if (is_search_upc) {
        if (is_search_upc_step2) {
            std::vector<AlbumCore::Track> tracks;
            neb::CJsonObject json(reply->readAll().toStdString());
            uint i = 0;
            std::string album_title, cover;
            json.Get("title", album_title);
            json["image"].Get("large", cover);
            do {
                AlbumCore::Track track;
                track.tid = json["tracks"]["items"][i]["id"].ToString();
                json["tracks"]["items"][i].Get("title", track.title);
                json["tracks"]["items"][i].Get("duration", track.duration);
                tracks.push_back(track);
            } while (!json["tracks"]["items"][++i].IsEmpty());
            emit searchByUPCFinished(tracks, album_title, cover);
        } else {
            neb::CJsonObject json(reply->readAll().toStdString());
            uint i = 0;
            std::string tmp;
            while (!json["albums"]["items"][i].IsEmpty()) {
                json["albums"]["items"][i].Get("upc", tmp);
                if (compareUPC(QString::fromStdString(this->upc), QString::fromStdString(tmp))) {
                    QUrl url("http://www.qobuz.com/api.json/0.2/album/get");
                    QUrlQuery query;
                    std::string album_id;
                    json["albums"]["items"][i].Get("id", album_id);
                    query.addQueryItem("album_id", QString::fromStdString(album_id));
                    query.addQueryItem("app_id", "950096963");
                    url.setQuery(query);
                    startRequest(url);
                    qDebug() << url;
                    is_search_upc_step2 = true;
                    break;
                }
                ++i;
            }
            if (!json["albums"]["items"][0].IsEmpty() && !is_search_upc_step2) {
                ++current_page;
                QUrl url("http://www.qobuz.com/api.json/0.2/album/search");
                QUrlQuery query;
                query.addQueryItem("query", upc_search_title);
                query.addQueryItem("limit", "500");
                query.addQueryItem("offset", QString::number(current_page * 500));
                query.addQueryItem("app_id", "950096963");
                url.setQuery(query);
                startRequest(url);
            } else if (!is_search_upc_step2) {
                std::vector<AlbumCore::Track> empty;
                emit searchByUPCFinished(empty, "", "");
            }
        }
    } else {
        neb::CJsonObject json(reply->readAll().toStdString());
        uint i = 0;
        std::string  tmp;
        std::string  keywords;
        bool hires = false;
        json.Get("query", keywords);
        while (!json["albums"]["items"][i].IsEmpty()) {
            AlbumCore::AlbumItem ai;
            json["albums"]["items"][i].Get("id" , tmp);
            ai.aid = QString::fromStdString(tmp);
            json["albums"]["items"][i].Get("title", tmp);
            ai.title = QString::fromStdString(tmp);
            json["albums"]["items"][i]["image"].Get("small", tmp);
            ai.cover = QString::fromStdString(tmp);
            json["albums"]["items"][i].Get("upc", tmp);
            while (tmp.length() < 15) {
                tmp.insert(0, 1, '0');
            }
            ai.upc = QString::fromStdString(tmp);
            json["albums"]["items"][i].Get("release_date_original", tmp);
            ai.date = QDate(QString::fromStdString(tmp).split('-')[0].toInt(), QString::fromStdString(tmp).split('-')[1].toInt(), QString::fromStdString(tmp).split('-')[2].toInt());
            json["albums"]["items"][i].Get("hires", hires);
            ai.hires = hires;
            AlbumCore::Album::getInstance().addAlbumItem(ai);
            ++i;
        }
        i = 0;
        while (!json["tracks"]["items"][i].IsEmpty()) {
            AlbumCore::AlbumItem ai;
            json["tracks"]["items"][i]["album"].Get("id", tmp);
            ai.aid = QString::fromStdString(tmp);
            json["tracks"]["items"][i]["album"].Get("title", tmp);
            ai.title = QString::fromStdString(tmp);
            json["tracks"]["items"][i]["album"]["image"].Get("small", tmp);
            ai.cover = QString::fromStdString(tmp);
            json["tracks"]["items"][i]["album"].Get("upc", tmp);
            while (tmp.length() < 15) {
                tmp.insert(0, 1, '0');
            }
            ai.upc = QString::fromStdString(tmp);
            json["tracks"]["items"][i]["album"].Get("release_date_original", tmp);
            ai.date = QDate(QString::fromStdString(tmp).split('-')[0].toInt(), QString::fromStdString(tmp).split('-')[1].toInt(), QString::fromStdString(tmp).split('-')[2].toInt());
            json["tracks"]["items"][i]["album"].Get("hires", hires);
            ai.hires = hires;
            AlbumCore::Album::getInstance().addAlbumItem(ai);
            ++i;
        }
        if (!json["tracks"]["items"][0].IsEmpty() && !json["albums"]["items"][0].IsEmpty()) {
            ++current_page;
            QUrl url("http://www.qobuz.com/api.json/0.2/catalog/search");
            QUrlQuery query;
            query.addQueryItem("query", QString::fromStdString(keywords));
            query.addQueryItem("limit", "500");
            query.addQueryItem("offset", QString::number(current_page * 500));
            query.addQueryItem("app_id", "950096963");
            url.setQuery(query);
            startRequest(url);
        } else {
            emit searchFinished(0x04);
        }
    }
    reply->deleteLater();
}

inline bool QobuzCore::Qobuz::compareUPC(QString a, QString b)
{
    if (a.contains(b)) {
        return true;
    } else if (b.contains(a)) {
        return true;
    } else {
        return false;
    }
}
