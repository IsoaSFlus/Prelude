#ifndef ALBUM_MODEL_H
#define ALBUM_MODEL_H

#include <QObject>
#include <QtCore>
#include <QtGui/QtGui>

#include "search_engine.h"

class Album
{
    Q_GADGET
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString cover READ cover)
    Q_PROPERTY(QString upc READ upc)
    Q_PROPERTY(QDate date READ date)
    Q_PROPERTY(QString id READ id)

public:
    Album(QString& id, QString title, QString cover, QDate& date, QString upc) {
        m_date = date;
        m_id = id;
        m_cover = cover;
        m_title = title;
        m_upc = upc;
    }
    Album() = default;
    Album(const Album& other)=default;
    Album& operator=(const Album& other)=default;

    const QDate& date() const {
        return m_date;
    }
    const QString& cover() {
        return m_cover;
    }
    const QString& title() const {
        return m_title;
    }
    const QString& id() const {
        return m_id;
    }
    const QString& upc() const {
        return m_upc;
    }


private:
    QDate m_date;
    QString m_title;
    QString m_cover;
    QString m_id;
    QString m_upc;
};

class Track {
    Q_GADGET
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(uint duration READ duration)
    Q_PROPERTY(QString id READ id)

public:
    Track(QString id, QString title, uint duration) {
        m_duration = duration;
        m_id = id;
        m_title = title;
    }
    Track() = default;
    Track(const Track& other)=default;
    Track& operator=(const Track& other)=default;

    const uint& duration() const {
        return m_duration;
    }
    const QString& title() const {
        return m_title;
    }
    const QString& id() const {
        return m_id;
    }

private:
    uint m_duration;
    QString m_title;
    QString m_id;
};

class AlbumModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStandardItemModel* model READ model)
    Q_PROPERTY(QStandardItemModel* track_model READ track_model)
    Q_DISABLE_COPY(AlbumModel)
public:
    explicit AlbumModel(QObject *parent = nullptr);

    QStandardItemModel* model() const;
    QStandardItemModel* track_model() const;

signals:
    void dataFetched();
    void dataTrackFetched(QVariant album_title, QVariant cover);

public slots:
    void search(QString keywords);
    void getTracks(QString upc, QString title);
    void addTracksToMPD(int index);

private:
    QStandardItemModel* m_model;
    QStandardItemModel* m_track_model;
    SearchEngineCore::SearchEngine* se;

private slots:
    void inputAlbumResults(std::map<QString, SearchEngineCore::Album> albums);
    void handleTrackResults(std::vector<TidalCore::Track> t, QString album_title, QString cover_large);
};

#endif // ALBUM_MODEL_H
