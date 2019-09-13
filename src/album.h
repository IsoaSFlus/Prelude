#ifndef ALBUM_H
#define ALBUM_H

#include <QtCore>
#include <unordered_map>

namespace std {
  template<> struct hash<QString> {
    std::size_t operator()(const QString& s) const {
      return qHash(s);
    }
  };
}

namespace AlbumCore {

struct AlbumItem
{
    QString aid;
    QString title;
    QString cover;
    QString cover_large;
    QString label;
    QString upc;
    QDate date;
};

class Album : public QObject
{
    Q_OBJECT
private:
    Album();
    ~Album();

    uint config_mask = 3; // bit 1 spotify bit 2 tidal
    uint finished_mask = 0;
    std::unordered_map<QString, AlbumItem> albums; // upc and album info

signals:
    void searchFinished();

public:
    static Album& getInstance()
    {
        static Album instance;
        return instance;
    }
    void addAlbumItem(AlbumItem& ai);
    const std::unordered_map<QString, AlbumItem>& getAlbums() const;
    void setSpotifyFinished();
    void setTidalFinished();
    void clear();
};
}
#endif
