#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "tidal_core.h"
#include "json/CJsonObject.hpp"
#include "album.h"


TidalCore::Tidal::Tidal(QObject *parent) : QObject(parent)
{
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Tidal::httpFinished);
}

void TidalCore::Tidal::search(QString keywords)
{
    clear();
    QUrl url("https://listen.tidal.com/v1/search");
    QUrlQuery query;
    query.addQueryItem("query", keywords);
    query.addQueryItem("limit", "9999");
    query.addQueryItem("offset", "0");
    query.addQueryItem("types", "ALBUMS,TRACKS");
    query.addQueryItem("countryCode", "US");
    url.setQuery(query);
    startRequest(url);
}

void TidalCore::Tidal::searchByUPC(QString upc, QString title)
{
    clear();
    is_search_upc = true;
    this->upc = upc.toStdString();
    QUrl url("https://listen.tidal.com/v1/search");
    QUrlQuery query;
//    title.remove('"');
    if (title.indexOf(' ', 25) == -1) {
        query.addQueryItem("query", title);
        qDebug() << title;
    } else {
        query.addQueryItem("query", title.left(title.indexOf(' ', 25)));
        qDebug() << title.left(title.indexOf(' ', 25));
    }
    query.addQueryItem("limit", "9999");
    query.addQueryItem("offset", "0");
    query.addQueryItem("types", "ALBUMS");
    query.addQueryItem("countryCode", "US");
    url.setQuery(query);
    startRequest(url);
}

void TidalCore::Tidal::queryAlbum()
{
    if (unchecked_albums.size() == 0) {
        emit searchFinished(0x02);
        return;
    }
    for (auto a : unchecked_albums) {
//        std::cout << a.first << a.second << std::endl;
        QUrl url("https://listen.tidal.com/v1/search");
        QUrlQuery query;
        query.addQueryItem("query", QString::fromStdString(a.second));
        query.addQueryItem("limit", "9999");
        query.addQueryItem("offset", "0");
        query.addQueryItem("types", "ALBUMS");
        query.addQueryItem("countryCode", "US");
        url.setQuery(query);
        startRequest(url);
    }

//    std::cout << request_count << std::endl;
}

void TidalCore::Tidal::startRequest(const QUrl &requestedUrl)
{
    QNetworkRequest qnr(requestedUrl);
    qnr.setRawHeader(QByteArray("x-tidal-token"), QByteArray("CzET4vdadNUFQ5JU"));
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));

    qnam->get(qnr);
}

void TidalCore::Tidal::clear()
{
    qnam->disconnect();
    qnam->deleteLater();
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Tidal::httpFinished);
    unchecked_albums.clear();
    detail_albums.clear();
    is_search_upc = false;
    is_search_upc_step2 = false;
    is_search_step1 = true;
    is_search_step2 = false;
    finished_count = 0;
    request_count = 0;
}

void TidalCore::Tidal::httpFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit searchFinished(0x02);
        clear();
        qDebug() << "Tidal search error: " << reply->error();
        reply->deleteLater();
        return;
    }
    if (is_search_upc) {
        if (is_search_upc_step2) {
            std::vector<AlbumCore::Track> tracks;
            neb::CJsonObject json(reply->readAll().toStdString());
            uint i = 0;
            std::string album_title, cover;
            json["rows"][0]["modules"][0]["album"].Get("title", album_title);
            json["rows"][0]["modules"][0]["album"].Get("cover", cover);
            do {
                AlbumCore::Track track;
                track.tid = json["rows"][1]["modules"][0]["pagedList"]["items"][i]["item"]["id"].ToString();
                json["rows"][1]["modules"][0]["pagedList"]["items"][i]["item"].Get("title", track.title);
                json["rows"][1]["modules"][0]["pagedList"]["items"][i]["item"].Get("duration", track.duration);
//                std::cout << track.tid << "-" << track.title << std::endl;
                tracks.push_back(track);
            } while (!json["rows"][1]["modules"][0]["pagedList"]["items"][++i].IsEmpty());
            emit searchByUPCFinished(tracks, album_title, cover);
        } else {
            neb::CJsonObject json(reply->readAll().toStdString());
            uint item_num = 0;
            std::string tmp;
            json["albums"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                json["albums"]["items"][i].Get("upc", tmp);
                if (this->upc.compare(tmp) == 0) {
                    QUrl url("https://listen.tidal.com/v1/pages/album");
                    QUrlQuery query;
                    query.addQueryItem("albumId", QString::fromStdString(json["albums"]["items"][i]["id"].ToString()));
                    query.addQueryItem("locale", "en_US");
                    query.addQueryItem("deviceType", "BROWSER");
                    query.addQueryItem("countryCode", "US");
                    url.setQuery(query);
                    startRequest(url);
                    is_search_upc_step2 = true;
                    break;
                }
            }
            if (!is_search_upc_step2) {
                std::vector<AlbumCore::Track> empty;
                emit searchByUPCFinished(empty, "", "");
            }
        }
    } else {
        if (is_search_step1 == true) {
            uint item_num = 0;
            std::string  tmp;
            int tmpid = 0;
            std::string  tmp_title;
            neb::CJsonObject json(reply->readAll().toStdString());
            json["albums"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                if (json["albums"]["items"][i]["explicit"].ToString().compare("true") == 0) {
                    continue;
                }
                AlbumCore::AlbumItem ai;
                json["albums"]["items"][i].Get("id" , tmpid);
                detail_albums.insert(tmpid);
                ai.aid = QString::number(tmpid);
                json["albums"]["items"][i].Get("title", tmp);
                ai.title = QString::fromStdString(tmp);
                json["albums"]["items"][i].Get("cover", tmp);
                ai.cover = "https://resources.tidal.com/images/" + QString::fromStdString(tmp).replace('-', '/') + "/320x320.jpg";
                json["albums"]["items"][i].Get("upc", tmp);
                ai.upc = QString::fromStdString(tmp);
                json["albums"]["items"][i].Get("releaseDate", tmp);
                ai.date = QDate(QString::fromStdString(tmp).split('-')[0].toInt(), QString::fromStdString(tmp).split('-')[1].toInt(), QString::fromStdString(tmp).split('-')[2].toInt());
                AlbumCore::Album::getInstance().addAlbumItem(ai);
            }
            json["tracks"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                json["tracks"]["items"][i]["album"].Get("id", tmpid);
                if (detail_albums.find(tmpid) == detail_albums.end() && unchecked_albums.find(tmpid) == unchecked_albums.end()) {
                    json["tracks"]["items"][i]["album"].Get("title", tmp_title);
                    unchecked_albums[tmpid] = tmp_title;
                }
            }
            is_search_step1 = false;
            queryAlbum();
        } else if (unchecked_albums.size() != 0) {
            neb::CJsonObject json(reply->readAll().toStdString());
            uint item_num = 0;
            int tmpid = 0;
            std::string tmp;
            json["albums"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                if (json["albums"]["items"][i]["explicit"].ToString().compare("true") == 0) {
                    continue;
                }
                AlbumCore::AlbumItem ai;
                json["albums"]["items"][i].Get("id" , tmpid);
                if (unchecked_albums.find(tmpid) == unchecked_albums.end()) {
                    continue;
                }
                ai.aid = QString::number(tmpid);
                json["albums"]["items"][i].Get("title", tmp);
                ai.title = QString::fromStdString(tmp);
                json["albums"]["items"][i].Get("cover", tmp);
                ai.cover = "https://resources.tidal.com/images/" + QString::fromStdString(tmp).replace('-', '/') + "/320x320.jpg";
                json["albums"]["items"][i].Get("upc", tmp);
                ai.upc = QString::fromStdString(tmp);
                json["albums"]["items"][i].Get("releaseDate", tmp);
                ai.date = QDate(QString::fromStdString(tmp).split('-')[0].toInt(), QString::fromStdString(tmp).split('-')[1].toInt(), QString::fromStdString(tmp).split('-')[2].toInt());
                AlbumCore::Album::getInstance().addAlbumItem(ai);
            }
            finished_count++;
            if (finished_count >= unchecked_albums.size()) {
                emit searchFinished(0x02);
            }
        } else {
            std::cout << "missed!" << std::endl;
        }
    }

    reply->deleteLater();
}



