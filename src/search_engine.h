#ifndef SEARCH_ENGINE_H
#define SEARCH_ENGINE_H

#include <QtCore>
#include <map>

#include "tidal/tidal_core.h"
#include "spotify/spotify_core.h"
#include "qobuz/qobuz_core.h"
#include "album.h"

namespace SearchEngineCore {


class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);
public slots:
    void findInTidal(QString upc, QString title);
    void findInQobuz(QString upc, QString title);
    void search(QString keywords);

signals:
    void findInQobuzReady(std::vector<AlbumCore::Track> t, QString album_title, QString cover_large);
    void searchFinished();


private:
    TidalCore::Tidal tidal;
    SpotifyCore::Spotify spotify;
    QobuzCore::Qobuz qobuz;
    uint config_mask = 5; // bit 1 spotify bit 2 tidal bit 3 qobuz
    uint finished_mask = 0;

private slots:
    void handleFIQResult(std::vector<AlbumCore::Track> t, std::string album_title, std::string cover);
    void setFinished(uint mask);
};

}
#endif // SEARCH_ENGINE_H
