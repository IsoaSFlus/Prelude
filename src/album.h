#ifndef ALBUM_H
#define ALBUM_H

#include <QtCore>
#include <unordered_map>

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
    bool hires = false;
};

struct Track
{
    std::string tid;
    std::string title;
    uint duration;
};

class Album : public QObject
{
    Q_OBJECT
private:
    Album();
    ~Album();

    std::unordered_map<QString, AlbumItem> albums; // upc and album info

signals:
    void searchFinished();

public:
    static Album& getInstance()
    {
        static Album instance;
        return instance;
    }
    void addAlbumItem(AlbumItem& ai, int from = 0); // 0 for tidal 1 for spotify
    const std::unordered_map<QString, AlbumItem>& getAlbums() const;
    void clear();
};
}
#endif
