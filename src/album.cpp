#include "album.h"
#include <unordered_map>
using namespace AlbumCore;

Album::Album() : QObject(nullptr)
{

}

Album::~Album()
{

}

void Album::addAlbumItem(AlbumItem &ai)
{
    albums[ai.upc] = ai;
}

const std::unordered_map<QString, AlbumItem> &Album::getAlbums() const
{
    return albums;
}

void Album::setSpotifyFinished()
{
    finished_mask = finished_mask | 0x01;
    if (finished_mask == config_mask) {
        finished_mask = 0;
        emit searchFinished();
    }
}

void Album::setTidalFinished()
{
    finished_mask = finished_mask | 0x02;
    if (finished_mask == config_mask) {
        finished_mask = 0;
        emit searchFinished();
    }
}

void Album::clear()
{
    config_mask = 3;
    finished_mask = 0;
    albums.clear();
}
