#include <iostream>
#include "search_engine.h"


SearchEngineCore::SearchEngine::SearchEngine(QObject *parent) : QObject(parent)
{
//    connect(&tidal, &TidalCore::Tidal::searchByUPCFinished, this, &SearchEngineCore::SearchEngine::handleFITResult);
    connect(&qobuz, &QobuzCore::Qobuz::searchByUPCFinished, this, &SearchEngineCore::SearchEngine::handleFIQResult);

    connect(&spotify, &SpotifyCore::Spotify::searchFinished, this, &SearchEngineCore::SearchEngine::setFinished);
    connect(&qobuz, &QobuzCore::Qobuz::searchFinished, this, &SearchEngineCore::SearchEngine::setFinished);
}

void SearchEngineCore::SearchEngine::findInTidal(QString upc, QString title)
{
    tidal.searchByUPC(upc, title);
}

void SearchEngineCore::SearchEngine::findInQobuz(QString upc, QString title)
{
    qobuz.searchByUPC(upc, title);
}

void SearchEngineCore::SearchEngine::search(QString keywords)
{
    if (keywords.left(4) == "sp::") {
        config_mask = 1;
        keywords.remove(0, 4);
        spotify.search(keywords);
    } else if (keywords.left(4) == "qo::") {
        config_mask = 4;
        keywords.remove(0, 4);
        qobuz.search(keywords);
    } else {
        config_mask = 5;
        qobuz.search(keywords);
        spotify.search(keywords);
    }

}


void SearchEngineCore::SearchEngine::handleFIQResult(std::vector<AlbumCore::Track> t, std::string album_title, std::string cover)
{
//    QString cover_large = "https://resources.tidal.com/images/" + QString::fromStdString(cover).replace('-', '/') + "/1280x1280.jpg";
    emit findInQobuzReady(t, QString::fromStdString(album_title), QString::fromStdString(cover));
}

void SearchEngineCore::SearchEngine::setFinished(uint mask)
{
    finished_mask = finished_mask | mask;
    if (finished_mask == config_mask) {
        finished_mask = 0;
        emit searchFinished();
    }
}
