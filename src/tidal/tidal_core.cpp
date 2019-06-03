#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "tidal_core.h"
#include "json/CJsonObject.hpp"


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

void TidalCore::Tidal::searchByUPC(std::string upc, std::string title)
{
    clear();
    is_search_upc = true;
    this->upc = upc;
    QUrl url("https://listen.tidal.com/v1/search");
    QUrlQuery query;
    query.addQueryItem("query", QString::fromStdString(title));
    query.addQueryItem("limit", "9999");
    query.addQueryItem("offset", "0");
    query.addQueryItem("types", "ALBUMS");
    query.addQueryItem("countryCode", "US");
    url.setQuery(query);
    startRequest(url);
}

void TidalCore::Tidal::queryAlbum()
{
    for (auto a : albums_map) {
        if (a.second.date.size() == 0) {
//            QUrl url("https://listen.tidal.com/v1/pages/album");
//            QUrlQuery query;
//            query.addQueryItem("albumId", QString::number(a.second.aid));
//            query.addQueryItem("locale", "en_US");
//            query.addQueryItem("deviceType", "BROWSER");
//            query.addQueryItem("countryCode", "US");
//            url.setQuery(query);
//            startRequest(url);

            QUrl url("https://listen.tidal.com/v1/search");
            QUrlQuery query;
            query.addQueryItem("query", QString::fromStdString(a.second.title));
            query.addQueryItem("limit", "9999");
            query.addQueryItem("offset", "0");
            query.addQueryItem("types", "ALBUMS");
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

    qnam->get(qnr);
}

void TidalCore::Tidal::sortResult()
{
    bool is_error = false;
    auto my_greater = [&](TidalCore::Album left, TidalCore::Album right) {
        if (left.date.size() < 3 || right.date.size() < 3) {
//            std::cout << left.aid << " " << left.title  << " " << left.date.size()  <<std::endl;
//            std::cout << right.aid << " " << right.title  << " " << right.date.size()  << std::endl;
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
        if (a.second.date.size() < 3) {
            continue;
        }
        detail_albums.push_back(a.second);
    }
    std::sort(detail_albums.begin(), detail_albums.end(), my_greater);
    if (is_error == true) {
        std::vector<TidalCore::Album> empty_album;
        emit searchFinished(empty_album);
        std::cout << "Tidal search failed!" << std::endl;
    } else {
        emit searchFinished(detail_albums);
    }
}

void TidalCore::Tidal::printResult()
{
    for (auto a : detail_albums) {
        std::string date = a.date[0].toStdString() + a.date[1].toStdString() + a.date[2].toStdString();
        std::string cover = "https://resources.tidal.com/images/" + QString::fromStdString(a.cover).replace('-', '/').toStdString() + "/1280x1280.jpg";
        std::cout << a.aid << " " << a.title << " " << a.upc << " " << cover << std::endl;
    }
}

void TidalCore::Tidal::clear()
{
    qnam->disconnect();
    qnam->deleteLater();
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Tidal::httpFinished);
    albums_map.clear();
    detail_albums.clear();
    is_search_upc = false;
    is_search_upc_step2 = false;
    finished_count = 0;
    request_count = 0;
}

void TidalCore::Tidal::httpFinished(QNetworkReply *reply)
{
    if (is_search_upc) {
        if (is_search_upc_step2) {
            std::vector<Track> tracks;
            neb::CJsonObject json(reply->readAll().toStdString());
            uint i = 0;
            std::string album_title, cover;
            json["rows"][0]["modules"][0]["album"].Get("title", album_title);
            json["rows"][0]["modules"][0]["album"].Get("cover", cover);
            do {
                TidalCore::Track track;
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
                std::vector<TidalCore::Track> empty;
                emit searchByUPCFinished(empty, "", "");
            }
        }
    } else {
        if (albums_map.size() == 0) {
            uint item_num = 0;
            std::string  tmp;
            neb::CJsonObject json(reply->readAll().toStdString());
            json["albums"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                if (json["albums"]["items"][i]["explicit"].ToString().compare("true") == 0) {
                    continue;
                }
                TidalCore::Album album;
                json["albums"]["items"][i].Get("id" , album.aid);
                json["albums"]["items"][i].Get("title", album.title);
                json["albums"]["items"][i].Get("cover", album.cover);
                json["albums"]["items"][i].Get("upc", album.upc);
                json["albums"]["items"][i].Get("releaseDate", tmp);
                album.date = QString::fromStdString(tmp).split('-');
                albums_map.insert({album.aid, album});
            }
            json["tracks"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                TidalCore::Album album;
                json["tracks"]["items"][i]["album"].Get("id", album.aid);
                json["tracks"]["items"][i]["album"].Get("title", album.title);
                json["tracks"]["items"][i]["album"].Get("cover", album.cover);
                albums_map.insert({album.aid, album});
            }
            queryAlbum();
        } else if (finished_count < request_count) {
            neb::CJsonObject json(reply->readAll().toStdString());
            uint item_num = 0;
            uint aid;
            std::string tmp;
            json["albums"].Get("totalNumberOfItems", item_num);
            for (uint i = 0; i < item_num; i++) {
                if (json["albums"]["items"][i]["explicit"].ToString().compare("true") == 0) {
                    continue;
                }
                json["albums"]["items"][i].Get("id", aid);
                if (albums_map.find(aid) != albums_map.end()) {
                    TidalCore::Album album;
                    json["albums"]["items"][i].Get("id" , album.aid);
                    json["albums"]["items"][i].Get("title", album.title);
                    json["albums"]["items"][i].Get("cover", album.cover);
                    json["albums"]["items"][i].Get("upc", album.upc);
                    json["albums"]["items"][i].Get("releaseDate", tmp);
                    album.date = QString::fromStdString(tmp).split('-');
                    albums_map[album.aid] = album;
                }
            }
            finished_count++;
            if (finished_count == request_count) {
                sortResult();
            }
        } else {
            std::cout << "missed!" << std::endl;
        }
    }

    reply->deleteLater();
}



