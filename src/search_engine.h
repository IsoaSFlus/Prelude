#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <QtCore>
#include <map>

#include "tidal/tidal_core.h"
#include "spotify/spotify_core.h"

namespace SearchEngineCore {

struct Album
{
    std::string aid;
    std::string title;
    std::string cover;
    std::string cover_large;
    std::string label;
    std::string upc;
    QDate date;
};


class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);

signals:
    void resultReady(std::vector<SearchEngineCore::Album> albums);
    void findInTidalReady(std::vector<TidalCore::Track> t)

public slots:
    void findInTidal(std::string upc, std::string title);

private:
    std::map<std::string, SearchEngineCore::Album> album_map; // pair of Album.upc and Album
    TidalCore::Tidal tidal;
    SpotifyCore::Spotify spotify;

private slots:
    void handleSpotifyResult(std::vector<SpotifyCore::Album> a);
    void handleTidalResult(std::vector<TidalCore::Album> a);
    void handleFITResult(std::vector<TidalCore::Track> t);
};

}
#endif // SEARCH_ENGINE_H
