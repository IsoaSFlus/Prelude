#include "album.h"
#include <unordered_map>
using namespace AlbumCore;

Album::Album() : QObject(nullptr)
{

}

Album::~Album()
{

}

void Album::addAlbumItem(AlbumItem &ai, int from)
{
    if (from != 0) {
        if (albums.find(ai.upc) != albums.end()) {
            return;
        }
    }
    albums[ai.upc] = ai;
}

const std::unordered_map<QString, AlbumItem> &Album::getAlbums() const
{
    return albums;
}

void Album::clear()
{
    albums.clear();
}
