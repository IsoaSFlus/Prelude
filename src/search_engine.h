#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <QtCore>
#include <map>

#include "tidal/tidal_core.h"
#include "spotify/spotify_core.h"

namespace SearchEngineCore {


class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);
public slots:
    void findInTidal(std::string upc, std::string title);
    void search(QString keywords);

signals:
    void findInTidalReady(std::vector<TidalCore::Track> t, QString album_title, QString cover_large);


private:
    TidalCore::Tidal tidal;
    SpotifyCore::Spotify spotify;

private slots:
    void handleFITResult(std::vector<TidalCore::Track> t, std::string album_title, std::string cover);
};

}
#endif // SEARCH_ENGINE_H
