#ifndef MPD_CLIENT_HPP
#define MPD_CLIENT_HPP

#include <QObject>
#include <QtCore>
#include <QTcpSocket>

enum MpdCmdType {
    CMD_NULL,
    CMD_ADD_TRACK,
    CMD_STATUS
};

class MPDClient : public QObject
{
    Q_OBJECT
public:
    static MPDClient& getInstance()
    {
        static MPDClient instance;
        return instance;
    }

    void init();
    void config(QString ip, uint port);
    void addTracks(QStringList value);

signals:

public slots:
    void readMPDSocket();
    void getStatus();
    void reconnect();

private:
    MPDClient(QObject *parent = nullptr);
    ~MPDClient();
    QTcpSocket* mpd_socket = nullptr;
    MpdCmdType current_cmd = CMD_NULL;
    QTimer* status_timer;
    QString ip;
    uint port = 6600;


};

#endif // MPD_CLIENT_H
