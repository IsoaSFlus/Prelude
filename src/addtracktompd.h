#ifndef ADDTRACKTOMPD_H
#define ADDTRACKTOMPD_H

#include <QObject>
#include <QNetworkAccessManager>

class AddTrackToMPD : public QObject
{
    Q_OBJECT

    QNetworkAccessManager* qnam;
    QString album_id;

public:
    explicit AddTrackToMPD(QString album_id, QString host, QString ip, QObject *parent = nullptr);
    void httpFinished(QNetworkReply *reply);
    void run();

};

#endif // ADDTRACKTOMPD_H
