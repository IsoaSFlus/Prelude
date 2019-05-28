#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "tidal_core.h"
#include "json/CJsonObject.hpp"


TidalCore::Tidal::Tidal(QObject *parent) : QObject(parent)
{
    connect(&qnam, &QNetworkAccessManager::finished, this, &Tidal::httpFinished);
}

void TidalCore::Tidal::search(QString keywords)
{
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

void TidalCore::Tidal::queryAlbum()
{
    for (auto a : albums_map) {
        if (a.second.date.size() == 0) {
            QUrl url("https://listen.tidal.com/v1/pages/album");
            QUrlQuery query;
            query.addQueryItem("albumId", QString::number(a.second.aid));
            query.addQueryItem("locale", "en_US");
            query.addQueryItem("deviceType", "BROWSER");
            query.addQueryItem("countryCode", "US");
            url.setQuery(query);
            startRequest(url);
            request_count++;
        }
    }
    if (request_count == 0) {
        sortResult();
//        printResult();
    }
//    std::cout << request_count << std::endl;
}

void TidalCore::Tidal::startRequest(const QUrl &requestedUrl)
{
    QNetworkRequest qnr(requestedUrl);
    qnr.setRawHeader(QByteArray("x-tidal-token"), QByteArray("CzET4vdadNUFQ5JU"));
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));

    qnam.get(qnr);
}

void TidalCore::Tidal::sortResult()
{
    bool is_error = false;
    auto my_greater = [&](TidalCore::Album left, TidalCore::Album right) {
        if (left.date.size() != 3 || right.date.size() != 3) {
            is_error = true;
            return false;
        }
        int l1 = left.date[0].toInt();
        int l2 = left.date[1].toInt();
        int l3 = left.date[2].toInt();
        int r1 = right.date[0].toInt();
        int r2 = right.date[1].toInt();
        int r3 = right.date[2].toInt();
        if (l1 == r1) {
            if (l2 == r2) {
                return l3 > r3;
            } else {
                return l2 > r2;
            }
        } else {
            return l1 > r1;
        }
    };
    for (auto a : albums_map) {
        detail_albums.push_back(a.second);
    }
    std::sort(detail_albums.begin(), detail_albums.end(), my_greater);
    if (is_error == true) {
        std::vector<TidalCore::Album> empty_album;
        emit searchFinished(empty_album);
    } else {
        emit searchFinished(detail_albums);
    }
}

void TidalCore::Tidal::printResult()
{
    for (auto a : detail_albums) {
        std::string date = a.date[0].toStdString() + a.date[1].toStdString() + a.date[2].toStdString();
        std::string cover = "https://resources.tidal.com/images/" + QString::fromStdString(a.cover).replace('-', '/').toStdString() + "/1280x1280.jpg";
        std::cout << a.aid << " " << a.title << " " << date << " " << cover << std::endl;
    }
    QCoreApplication::exit(0);
}

void TidalCore::Tidal::clear()
{
    albums_map.clear();
    detail_albums.clear();
    finished_count = 0;
    request_count = 0;
}

void TidalCore::Tidal::httpFinished(QNetworkReply *reply)
{
    if (albums_map.size() == 0) {
        uint item_num = 0;
        std::string  tmp;
        neb::CJsonObject json(reply->readAll().toStdString());
        json["albums"].Get("totalNumberOfItems", item_num);
        for (uint i = 0; i < item_num; i++) {
            TidalCore::Album album;
            json["albums"]["items"][i].Get("id" , album.aid);
            json["albums"]["items"][i].Get("title", album.title);
            json["albums"]["items"][i].Get("cover", album.cover);
            json["albums"]["items"][i].Get("releaseDate", tmp);
            album.date = QString::fromStdString(tmp).split('-');
            albums_map.insert(std::pair<uint, TidalCore::Album>(album.aid, album));
        }
        json["tracks"].Get("totalNumberOfItems", item_num);
        for (uint i = 0; i < item_num; i++) {
            TidalCore::Album album;
            json["tracks"]["items"][i]["album"].Get("id", album.aid);
            json["tracks"]["items"][i]["album"].Get("title", album.title);
            json["tracks"]["items"][i]["album"].Get("cover", album.cover);
            albums_map.insert(std::pair<uint, TidalCore::Album>(album.aid, album));
        }
        queryAlbum();
    } else if (finished_count < request_count) {
        neb::CJsonObject json(reply->readAll().toStdString());
        TidalCore::Album album;
        std::string  tmp;
        json["rows"][0]["modules"][0]["album"].Get("id", album.aid);
        json["rows"][0]["modules"][0]["album"].Get("title", album.title);
        json["rows"][0]["modules"][0]["album"].Get("cover", album.cover);
        json["rows"][0]["modules"][0]["album"].Get("releaseDate", tmp);
        album.date = QString::fromStdString(tmp).split('-');
        albums_map[album.aid] = album;
        finished_count++;
        if (finished_count == request_count) {
            sortResult();
//            printResult();
        }
    } else {
        std::cout << "missed!" << std::endl;
    }
    reply->deleteLater();
}



