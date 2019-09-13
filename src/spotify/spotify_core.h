#ifndef SPOTIFY_CORE_H
#define SPOTIFY_CORE_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <unordered_map>
#include <unordered_set>

namespace SpotifyCore {

//struct Album
//{
//    std::string aid;
//    std::string title;
//    std::string cover;
//    std::string label;
//    std::string upc;
//    QStringList date;
//};

class Spotify : public QObject
{
    Q_OBJECT
public:
    explicit Spotify(QObject *parent = nullptr);

    void search(QString keywords);
    void getToken();
    void queryAlbum();
    void nextPage(int offset);
    void startRequest(const QUrl &requestedUrl);
//    void printResult();
//    void sortResult();
    void clear();

signals:
//    void searchFinished(std::vector<Album>);

private:
    QNetworkAccessManager* qnam = nullptr;
    QNetworkAccessManager* token_nam = nullptr;
    QDateTime token_datetime;
    QByteArray token;
    std::unordered_set<std::string> unchecked_albums;
    QString keywords;
    bool first_request = true;
    bool detail_request = false;
    uint finished_count = 0;
//    uint request_count = 0;
    int total_tracks = 0;
    int total_albums = 0;

private slots:
    void httpFinished(QNetworkReply *reply);
    void getTokenFinished(QNetworkReply *reply);
};

}
#endif // SPOTIFY_CORE_H
