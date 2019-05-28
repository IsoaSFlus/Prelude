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
    connect(qnam, &QNetworkAccessManager::finished, this, &Spotify::httpFinished);
}

void SpotifyCore::Spotify::search(QString keywords)
{
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
    qnr.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer BQAV87nepxwECa0NCy09of1x7kjKA0ZcKuuhxFRuyoLXGTr_UFl6Qixg2UY5m0uvT6QZ8QcprDmj6Kk52-Y"));
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

    if (detail_request) {
        neb::CJsonObject json(reply->readAll().toStdString());
        SpotifyCore::Album album;
        album.aid = json["id"].ToString();
        album.title = json["name"].ToString();
        album.upc = json["external_ids"]["upc"].ToString();
        album.cover = json["images"][1]["url"].ToString();
        album.label = json["label"].ToString();
        album.date = QString::fromStdString(json["release_date"].ToString()).split('-');
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
        if (offset+50 >= total_albums && offset+50 >= total_tracks) {
            queryAlbum();
            detail_request = true;
        } else {
            nextPage(offset + 50);
        }
    }
    reply->deleteLater();
}
