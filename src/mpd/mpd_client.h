#ifndef MPD_CLIENT_HPP
#define MPD_CLIENT_HPP

#include <QObject>
#include <mpd/client.h>

class MPDClient : public QObject
{
    Q_OBJECT
public:
    explicit MPDClient(QObject *parent = nullptr);
    ~MPDClient();

signals:

public slots:

private:
    struct mpd_connection* conn = nullptr;

    void printPlaylist();
};

#endif // MPD_CLIENT_H
