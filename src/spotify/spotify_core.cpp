#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>
#include <algorithm>

#include "json/CJsonObject.hpp"
#include "spotify_core.h"
#include "album.h"


SpotifyCore::Spotify::Spotify(QObject *parent) : QObject(parent)
{
    qnam = new QNetworkAccessManager(this);
    token_nam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Spotify::httpFinished);
    connect(token_nam, &QNetworkAccessManager::finished, this, &Spotify::getTokenFinished);

    token_datetime = QDateTime::currentDateTime().addSecs(-3600);
//    token_lifetime = new QTimer(this);
//    token_lifetime->setInterval(10000);
//    connect(token_lifetime, &QTimer::timeout, this, &Spotify::tokenTick);
//    getToken();
}

void SpotifyCore::Spotify::search(QString keywords)
{
    clear();
    this->keywords = keywords;
    if (token_datetime.addSecs(1500) < QDateTime::currentDateTime()) {
        getToken();
        return;
    }
    QUrl url("https://api.spotify.com/v1/search");
    QUrlQuery query;
    query.addQueryItem("query", keywords);
    query.addQueryItem("limit", "50");
    query.addQueryItem("offset", "0");
    query.addQueryItem("type", "album,track");
    query.addQueryItem("market", "HK");
//    query.addQueryItem("market", "JP");
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
    qnr.setRawHeader(QByteArray("Authorization"), QByteArray("Basic Y2E1YTU4Zjk4ZTBkNGJjNmE2ZThhMmZkNWIzZTg5NWQ6YWJjZTQzZDllNmNkNDZhOTgyNDFlZTI1NDFhZWZjMWU="));
    token_nam->post(qnr, data);
}

void SpotifyCore::Spotify::queryAlbum()
{
    if (unchecked_albums.size() == 0) {
        emit searchFinished(0x01);
        return;
    }
    for (auto a : unchecked_albums) {
        QUrl url(QString("https://api.spotify.com/v1/albums/") + QString::fromStdString(a));
        startRequest(url);
    }
//    request_count = static_cast<uint>(albums_set.size());
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

//void SpotifyCore::Spotify::printResult()
//{
//    for (auto a : ) {
//        std::cout << a.aid << " " << a.upc << " " << a.cover << " " << a.label << std::endl;
//    }
//}

void SpotifyCore::Spotify::clear()
{
    qnam->disconnect();
    qnam->deleteLater();
    qnam = new QNetworkAccessManager(this);
    connect(qnam, &QNetworkAccessManager::finished, this, &Spotify::httpFinished);
    unchecked_albums.clear();
    keywords.clear();
    first_request = true;
    detail_request = false;
    finished_count = 0;
//    request_count = 0;
    total_tracks = 0;
    total_albums = 0;
}

void SpotifyCore::Spotify::httpFinished(QNetworkReply *reply)
{
//    if (token.isEmpty()) {
//        emit searchFinished(detail_albums);
//        reply->deleteLater();
//        return;
//    }
    if (reply->error() != QNetworkReply::NoError) {
        emit searchFinished(0x01);
        clear();
        qDebug() << "Spotify search error: " << reply->error();
        reply->deleteLater();
        return;
    }
    if (detail_request) {
        neb::CJsonObject json(reply->readAll().toStdString());
        std::string tmp;
        AlbumCore::AlbumItem ai;
        json.Get("id", tmp);
        ai.aid = QString::fromStdString(tmp);
        json.Get("name", tmp);
        ai.title = QString::fromStdString(tmp);
        json["external_ids"].Get("upc", tmp);
        while (tmp.length() < 15) {
            tmp.insert(0, 1, '0');
        }
        ai.upc = QString::fromStdString(tmp);
        json["images"][1].Get("url",tmp);
        ai.cover = QString::fromStdString(tmp);
        json.Get("label", tmp);
        ai.label = QString::fromStdString(tmp);
        json.Get("release_date", tmp);
        QStringList tmpsl(QString::fromStdString(tmp).split('-'));
        if (tmpsl.size() < 3) {
            tmpsl << "01" << "01";
        }
        ai.date = QDate(tmpsl[0].toInt(), tmpsl[1].toInt(), tmpsl[2].toInt());
        AlbumCore::Album::getInstance().addAlbumItem(ai, 1);
        finished_count++;
        if (finished_count >= unchecked_albums.size()) {
            emit searchFinished(0x01);
        }
    } else {
        neb::CJsonObject json(reply->readAll().toStdString());
//        std::cout << json.ToFormattedString();
        if (first_request) {
            json["albums"].Get("total", total_albums);
            json["tracks"].Get("total", total_tracks);
            first_request = false;
        }

        uint array_size = static_cast<uint>(json["albums"]["items"].GetArraySize());
        std::string tmp;
//        std::cout << array_size << std::endl;
        for (uint i = 0; i < array_size; i++) {
            json["albums"]["items"][i].Get("id", tmp);
            unchecked_albums.insert(tmp);
        }

        array_size = static_cast<uint>(json["tracks"]["items"].GetArraySize());
        for (uint i = 0; i < array_size; i++) {
            json["tracks"]["items"][i]["album"].Get("id", tmp);
            unchecked_albums.insert(tmp);
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
    if (reply->error() != QNetworkReply::NoError) {
        emit searchFinished(0x01);
        clear();
        qDebug() << "Cannot get spotify token: " << reply->error();
        reply->deleteLater();
        return;
    }
    neb::CJsonObject json(reply->readAll().toStdString());
    std::string tmp;
    json.Get("access_token", tmp);
    token = QByteArray("Bearer ") + QByteArray::fromStdString(tmp);
    token_datetime = QDateTime::currentDateTime();
    search(this->keywords);
    qDebug() << "token: " << token;
    reply->deleteLater();
}
