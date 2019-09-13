#include <iostream>
#include "search_engine.h"


SearchEngineCore::SearchEngine::SearchEngine(QObject *parent) : QObject(parent)
{
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
}


void SearchEngineCore::SearchEngine::handleFITResult(std::vector<TidalCore::Track> t, std::string album_title, std::string cover)
{
    QString cover_large = "https://resources.tidal.com/images/" + QString::fromStdString(cover).replace('-', '/') + "/1280x1280.jpg";
    emit findInTidalReady(t, QString::fromStdString(album_title), cover_large);
}
