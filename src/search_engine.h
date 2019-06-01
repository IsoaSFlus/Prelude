#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <QtCore>
#include <map>

#include "tidal/tidal_core.h"
#include "spotify/spotify_core.h"

namespace SearchEngineCore {

struct Album
{
    QString aid;
    QString title;
    QString cover;
    QString cover_large;
    QString label;
    QString upc;
    QDate date;
};


class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);
public slots:
    void findInTidal(std::string upc, std::string title);
    void search(QString keywords);

signals:
    void resultReady(std::map<QString, SearchEngineCore::Album> albums);
    void findInTidalReady(std::vector<TidalCore::Track> t, QString album_title, QString cover_large);


private:
    std::map<QString, SearchEngineCore::Album> album_map; // pair of Album.upc and Album
    TidalCore::Tidal tidal;
    SpotifyCore::Spotify spotify;
    bool spotify_ready = false;
    bool tidal_ready = false;

private slots:
    void handleSpotifyResult(std::vector<SpotifyCore::Album> a);
    void handleTidalResult(std::vector<TidalCore::Album> a);
    void handleFITResult(std::vector<TidalCore::Track> t, std::string album_title, std::string cover);
};

}
#endif // SEARCH_ENGINE_H
