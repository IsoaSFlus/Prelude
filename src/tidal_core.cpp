#include <QtNetwork>
#include <QtCore>
#include <QUrl>
#include <iostream>

#include "tidal_core.h"
#include "CJsonObject.hpp"


TidalCore::Tidal::Tidal(QObject *parent) : QObject(parent)
{
    QUrl url("https://listen.tidal.com/v1/search");
    QUrlQuery query;
    std::string keywords;
    std::cout << "Input search string:";
    std::cin >> keywords;
    query.addQueryItem("query", QString::fromStdString(keywords));
    query.addQueryItem("limit", "9999");
    query.addQueryItem("offset", "0");
    query.addQueryItem("types", "ALBUMS,TRACKS");
    query.addQueryItem("countryCode", "US");
    url.setQuery(query);
    startRequest(url);
}

void TidalCore::Tidal::startRequest(const QUrl &requestedUrl)
{
    QNetworkRequest qnr(requestedUrl);
    qnr.setRawHeader(QByteArray("x-tidal-token"), QByteArray("CzET4vdadNUFQ5JU"));
    qnr.setRawHeader(QByteArray("user-agent"), QByteArray("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36"));

    qnam.get(qnr);
    connect(&qnam, &QNetworkAccessManager::finished, this, &Tidal::httpFinished);
}

void TidalCore::Tidal::httpFinished(QNetworkReply *reply)
{
    uint item_num = 0;
    uint aid = 0;
    std::string title;
    neb::CJsonObject json(reply->readAll().toStdString());
    json["albums"].Get("totalNumberOfItems", item_num);
    for (uint i = 0; i < item_num; i++) {
        json["albums"]["items"][i].Get("id", aid);
        json["albums"]["items"][i].Get("title", title);
        album.insert(std::pair<uint, QString>(aid, QString::fromStdString(title)));
    }
    json["tracks"].Get("totalNumberOfItems", item_num);
    for (uint i = 0; i < item_num; i++) {
        json["tracks"]["items"][i]["album"].Get("id", aid);
        json["tracks"]["items"][i]["album"].Get("title", title);
        album.insert(std::pair<uint, QString>(aid, QString::fromStdString(title)));
    }
    for (auto a : album) {
        std::cout << a.first << " " << a.second.toStdString() << std::endl;
    }

    reply->deleteLater();
    QCoreApplication::exit(0);
}



