#include "mpd_client.h"

MPDClient::MPDClient(QObject *parent) : QObject(parent)
{
    conn = mpd_connection_new("127.0.0.1", 6600, 1000);
//    printPlaylist();
}

MPDClient::~MPDClient()
{
    mpd_connection_free(conn);
}

void MPDClient::printPlaylist()
{
    mpd_send_list_queue_meta(conn);

    struct mpd_song *song;
    while ((song = mpd_recv_song(conn)) != NULL) {
        printf("%s", mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
        mpd_song_free(song);

        printf("\n");
    }

    mpd_response_finish(conn);
}
