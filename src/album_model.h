#ifndef ALBUM_MODEL_H
#define ALBUM_MODEL_H

#include <QObject>
#include <QtCore>
#include <QtGui/QtGui>

#include "tidal/tidal_core.h"

class Album
{
    Q_GADGET
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString cover READ cover)
    Q_PROPERTY(QDate date READ date)
    Q_PROPERTY(uint id READ id)

public:
    Album(uint& id, QString title, QString cover, QDate& date) {
        m_date = date;
        m_id = id;
        m_cover = cover;
        m_cover.replace('-', '/');
        m_title = title;
    }
    Album() = default;
    Album(const Album& other)=default;
    Album& operator=(const Album& other)=default;

    const QDate& date() const {
        return m_date;
    }
    const QString cover() {
        return QString("https://resources.tidal.com/images/") + m_cover + QString("/160x160.jpg");
    }
    const QString& title() const {
        return m_title;
    }
    const uint& id() const {
        return m_id;
    }

private:
    QDate m_date;
    QString m_title;
    QString m_cover;
    uint m_id = 0;
};

class AlbumModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStandardItemModel* model READ model)
    Q_DISABLE_COPY(AlbumModel)
public:
    explicit AlbumModel(QObject *parent = nullptr);

    QStandardItemModel* model() const;

public slots:
    void search(QString keywords);

private:
    QStandardItemModel* m_model;
    TidalCore::Tidal tidal;

private slots:
    void inputAlbumResults(std::vector<TidalCore::Album> albums);
};

#endif // ALBUM_MODEL_H
