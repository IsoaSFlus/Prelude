#ifndef QOBUZ_CORE_H
#define QOBUZ_CORE_H

#include <QNetworkAccessManager>
#include <unordered_map>
#include <unordered_set>

#include "album.h"


namespace QobuzCore {

class Qobuz : public QObject
{
    Q_OBJECT
public:
    explicit Qobuz(QObject *parent = nullptr);

    void search(QString keywords);
    void searchByUPC(QString upc, QString title);
    void queryAlbum();
    void startRequest(const QUrl &requestedUrl);
    void sortResult();
    void printResult();
    void clear();

signals:
    void searchByUPCFinished(std::vector<AlbumCore::Track>, std::string, std::string);
    void searchFinished(uint mask);

private:
    QNetworkAccessManager* qnam;
    std::unordered_set<int> detail_albums;
    bool is_search_upc = false;
    bool is_search_upc_step2 = false;
    QString upc_search_title;
    std::string upc;
    int current_page = 0;

    void httpFinished(QNetworkReply *reply);
    bool compareUPC(QString a, QString b);
};

}
#endif // QOBUZ_CORE_H
