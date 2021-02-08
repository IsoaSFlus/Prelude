#include "mpd_client.h"

MPDClient::MPDClient(QObject *parent) :
    QObject(parent),
    ip("127.0.0.1")
{
    status_timer = new QTimer(this);
    mpd_socket = new QTcpSocket(this);
    connect(mpd_socket, &QTcpSocket::readyRead, this, &MPDClient::readMPDSocket);
    connect(status_timer, &QTimer::timeout, this, &MPDClient::getStatus);
    status_timer->start(1000);
}

MPDClient::~MPDClient()
{

}

void MPDClient::init()
{
    mpd_socket->connectToHost(ip, port);
}

void MPDClient::config(QString ip, uint port)
{
    this->ip = ip;
    this->port = port;
    init();
}

void MPDClient::addTracks(QStringList value)
{
    if (current_cmd == CMD_NULL && mpd_socket->state() == QTcpSocket::ConnectedState) {
        current_cmd = CMD_ADD_TRACK;
        QString cmd;
        cmd += "command_list_begin\n";
        for (const auto& s : value) {
            qDebug().noquote() << s;
            cmd += "add " + s + "\n";
        }
        cmd += "command_list_end\n";
        mpd_socket->write(cmd.toUtf8());
    } else {
        QTimer::singleShot(1000, std::bind(&MPDClient::addTracks, this, value));
    }
}

void MPDClient::readMPDSocket()
{

    if (current_cmd == CMD_ADD_TRACK) {
        while (mpd_socket->canReadLine()) {
//            qDebug() << mpd_socket->readLine();
            mpd_socket->readLine();
        }
        current_cmd = CMD_NULL;
    } else if (current_cmd == CMD_STATUS) {
        while (mpd_socket->canReadLine()) {
//            qDebug() << mpd_socket->readLine();
            mpd_socket->readLine();
        }
        current_cmd = CMD_NULL;
    } else {
        while (mpd_socket->canReadLine()) {
//            qDebug() << mpd_socket->readLine();
            mpd_socket->readLine();
        }
    }
}

void MPDClient::getStatus()
{
    if (mpd_socket->state() == QTcpSocket::UnconnectedState) {
        init();
        return;
    }
    if (current_cmd == CMD_NULL && mpd_socket->state() == QTcpSocket::ConnectedState) {
        current_cmd = CMD_STATUS;
        QString cmd;
        cmd = "command_list_begin\n"
               "currentsong\n"
               "status\n"
               "command_list_end\n";
        mpd_socket->write(cmd.toUtf8());
    }
}

void MPDClient::reconnect()
{
//    mpd_send_list_queue_meta(conn);

//    struct mpd_song *song;
//    while ((song = mpd_recv_song(conn)) != NULL) {
//        printf("%s", mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
//        mpd_song_free(song);

//        printf("\n");
//    }

//    mpd_response_finish(conn);
}
