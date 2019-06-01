#include <iostream>
#include "search_engine.h"


SearchEngineCore::SearchEngine::SearchEngine(QObject *parent) : QObject(parent)
{
    connect(&tidal, &TidalCore::Tidal::searchFinished, this, &SearchEngineCore::SearchEngine::handleTidalResult);
    connect(&spotify, &SpotifyCore::Spotify::searchFinished, this, &SearchEngineCore::SearchEngine::handleSpotifyResult);
    connect(&tidal, &TidalCore::Tidal::searchByUPCFinished, this, &SearchEngineCore::SearchEngine::handleFITResult);
}

void SearchEngineCore::SearchEngine::findInTidal(std::string upc, std::string title)
{
    tidal.searchByUPC(upc, title);
}

void SearchEngineCore::SearchEngine::search(QString keywords)
{
    tidal.search(keywords);
    spotify.search(keywords);
    album_map.clear();
    spotify_ready = false;
    tidal_ready = false;
}

void SearchEngineCore::SearchEngine::handleSpotifyResult(std::vector<SpotifyCore::Album> a)
{
    for (auto album : a) {
        SearchEngineCore::Album album_s;
        album_s.aid = QString::fromStdString(album.aid);
        album_s.title = QString::fromStdString(album.title);
        album_s.cover = QString::fromStdString(album.cover);
        album_s.label = QString::fromStdString(album.label);
        album_s.upc = QString::fromStdString(album.upc);
        QDate d(album.date[0].toInt(), album.date[1].toInt(), album.date[2].toInt());
        album_s.date = d;
        album_map.insert({album_s.upc, album_s});
    }
    spotify_ready = true;
    if (tidal_ready == true && spotify_ready == true) {
        emit resultReady(album_map);
    }
}

void SearchEngineCore::SearchEngine::handleTidalResult(std::vector<TidalCore::Album> a)
{
    for (auto album : a) {
        SearchEngineCore::Album album_s;
        album_s.aid = QString::number(album.aid);
        album_s.title = QString::fromStdString(album.title);
        album_s.cover = "https://resources.tidal.com/images/" + QString::fromStdString(album.cover).replace('-', '/') + "/320x320.jpg";
        album_s.label = QString::fromStdString(album.label);
        album_s.upc = QString::fromStdString(album.upc);
        QDate d(album.date[0].toInt(), album.date[1].toInt(), album.date[2].toInt());
        album_s.date = d;
        album_map[album_s.upc] = album_s;
//        album_map.insert({album_s.upc, album_s});
    }
    tidal_ready = true;
    if (tidal_ready == true && spotify_ready == true) {
        emit resultReady(album_map);
    }
}

void SearchEngineCore::SearchEngine::handleFITResult(std::vector<TidalCore::Track> t, std::string album_title, std::string cover)
{
    QString cover_large = "https://resources.tidal.com/images/" + QString::fromStdString(cover).replace('-', '/') + "/1280x1280.jpg";
    emit findInTidalReady(t, QString::fromStdString(album_title), cover_large);
}
