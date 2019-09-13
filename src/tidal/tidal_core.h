#ifndef TIDAL_CORE_H
#define TIDAL_CORE_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <unordered_map>
#include <unordered_set>

namespace TidalCore {

//struct Album
//{
//    uint aid;
//    std::string title;
//    std::string cover;
//    std::string label;
//    std::string upc;
//    QStringList date;
//};

struct Track
{
    std::string tid;
    std::string title;
    uint duration;
};

class Tidal : public QObject {
    Q_OBJECT

public:
    explicit Tidal(QObject *parent = nullptr);

    void search(QString keywords);
    void searchByUPC(std::string upc, std::string title);
    void queryAlbum();
    void startRequest(const QUrl &requestedUrl);
    void sortResult();
    void printResult();
    void clear();

signals:
//    void searchFinished(std::vector<Album>);
    void searchByUPCFinished(std::vector<Track>, std::string, std::string);

private:
    QNetworkAccessManager* qnam;
//    std::map<uint, TidalCore::Album> albums_map;
//    std::vector<Album> detail_albums;
    std::unordered_set<int> detail_albums;
    std::unordered_map<int, std::string> unchecked_albums; // aid, title
    bool is_search_upc = false;
    bool is_search_upc_step2 = false;
    bool is_search_step1 = true;
    bool is_search_step2 = false;
    std::string upc;
    uint finished_count = 0;
    uint request_count = 0;

private slots:
    void httpFinished(QNetworkReply *reply);
//    void httpReadyRead();

};

}


#endif // TIDAL_CORE_H
