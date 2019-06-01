#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "json/CJsonObject.hpp"
#include "spotify_core.h"


SpotifyCore::Spotify::Spotify(QObject *parent) : QObject(parent)
{
    qnam = new QNetworkAccessManager(this);
    token_nam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Spotify::httpFinished);
    connect(token_nam, &QNetworkAccessManager::finished, this, &Spotify::getTokenFinished);

    token_lifetime = new QTimer(this);
    token_lifetime->setInterval(10000);
    connect(token_lifetime, &QTimer::timeout, this, &Spotify::tokenTick);
    getToken();
}

void SpotifyCore::Spotify::search(QString keywords)
{
    clear();
    this->keywords = keywords;
    QUrl url("https://api.spotify.com/v1/search");
    QUrlQuery query;
    query.addQueryItem("query", keywords);
    query.addQueryItem("limit", "50");
    query.addQueryItem("offset", "0");
    query.addQueryItem("type", "album,track");
    url.setQuery(query);
    startRequest(url);
}

void SpotifyCore::Spotify::getToken()
{
    QUrl url("https://accounts.spotify.com/api/token");
    QNetworkRequest qnr(url);
    QByteArray data;
    data.append("grant_type=client_credentials");
    qnr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    qnr.setRawHeader(QByteArray("Authorization"), QByteArray("Basic Y2E1YTU4Zjk4ZTBkNGJjNmE2ZThhMmZkNWIzZTg5NWQ6MmY2NjVjOGIxYzQ1NGQ4YTlhYjkwNGJmMWU1M2VkMjg="));
    token_nam->post(qnr, data);
}

void SpotifyCore::Spotify::queryAlbum()
{
    for (auto a : albums_set) {
        QUrl url(QString("https://api.spotify.com/v1/albums/") + QString::fromStdString(a));
        startRequest(url);
    }
    request_count = static_cast<uint>(albums_set.size());
}

void SpotifyCore::Spotify::nextPage(int offset)
{
    QUrl url("https://api.spotify.com/v1/search");
    QUrlQuery query;
    query.addQueryItem("query", keywords);
    query.addQueryItem("limit", "50");
    query.addQueryItem("offset", QString::number(offset));
    query.addQueryItem("type", "album,track");
    url.setQuery(query);
    startRequest(url);
}

void SpotifyCore::Spotify::startRequest(const QUrl &requestedUrl)
{
    QNetworkRequest qnr(requestedUrl);
    qnr.setRawHeader(QByteArray("Authorization"), token);
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));
    qnam->get(qnr);
}

void SpotifyCore::Spotify::printResult()
{
    for (auto a : detail_albums) {
        std::cout << a.aid << " " << a.upc << " " << a.cover << " " << a.label << std::endl;
    }
}

void SpotifyCore::Spotify::clear()
{
    delete qnam;
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Spotify::httpFinished);
    albums_set.clear();
    detail_albums.clear();
    keywords.clear();
    first_request = true;
    detail_request = false;
    finished_count = 0;
    request_count = 0;
    total_tracks = 0;
    total_albums = 0;
}

void SpotifyCore::Spotify::httpFinished(QNetworkReply *reply)
{
    if (token.isEmpty()) {
        emit searchFinished(detail_albums);
        reply->deleteLater();
        return;
    }
    if (detail_request) {
        neb::CJsonObject json(reply->readAll().toStdString());
        SpotifyCore::Album album;
        std::string tmp;
        json.Get("id", album.aid);
        json.Get("name", album.title);
        json["external_ids"].Get("upc", album.upc);
        json["images"][1].Get("url", album.cover);
        json.Get("label", album.label);
        json.Get("release_date", tmp);
        album.date = QString::fromStdString(tmp).split('-');
        if (album.date.size() < 3) {
            album.date << "01" << "01";
        }
        detail_albums.push_back(album);
        finished_count++;
        if (finished_count == request_count) {
//            printResult();
            emit searchFinished(detail_albums);
        }
    } else {
        neb::CJsonObject json(reply->readAll().toStdString());
        if (first_request) {
            json["albums"].Get("total", total_albums);
            json["tracks"].Get("total", total_tracks);
            first_request = false;
        }

        uint array_size = static_cast<uint>(json["albums"]["items"].GetArraySize());
        std::string tmp;
        for (uint i = 0; i < array_size; i++) {
            json["albums"]["items"][i].Get("id", tmp);
            albums_set.insert(tmp);
        }

        array_size = static_cast<uint>(json["tracks"]["items"].GetArraySize());
        for (uint i = 0; i < array_size; i++) {
            json["tracks"]["items"][i]["album"].Get("id", tmp);
            albums_set.insert(tmp);
        }

        int offset = 0;
        json["tracks"].Get("offset", offset);
//        std::cout << offset << " " << total_albums << " " << total_tracks << std::endl;
        if (offset+50 >= total_albums && offset+50 >= total_tracks) {
            queryAlbum();
            detail_request = true;
        } else {
            nextPage(offset + 50);
        }
    }
    reply->deleteLater();
}

void SpotifyCore::Spotify::getTokenFinished(QNetworkReply *reply)
{
    neb::CJsonObject json(reply->readAll().toStdString());
    std::string tmp;
    json.Get("access_token", tmp);
    token = QByteArray("Bearer ") + QByteArray::fromStdString(tmp);
    qDebug() << "token: " << token;
}

void SpotifyCore::Spotify::tokenTick()
{
    token_count++;
    if (token_count == 180) {
        token_count = 0;
        getToken();
    }
}
