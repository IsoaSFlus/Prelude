#include "album_model.h"

AlbumModel::AlbumModel(QObject *parent) : QObject(parent)
{
    m_model = new QStandardItemModel(this);

    connect(&tidal, &TidalCore::Tidal::searchFinished, this, &AlbumModel::inputAlbumResults);
}

QStandardItemModel *AlbumModel::model() const
{
    return m_model;
}

void AlbumModel::search(QString keywords)
{
    tidal.clear();
    m_model->clear();
    m_model->insertColumn(0);
    tidal.search(keywords);
}

void AlbumModel::inputAlbumResults(std::vector<TidalCore::Album> albums)
{
    for (auto album : albums) {
        const int new_row= m_model->rowCount();
        QDate d(album.date[0].toInt(), album.date[1].toInt(), album.date[2].toInt());
        const Album a(album.aid, QString::fromStdString(album.title), QString::fromStdString(album.cover), d);
        m_model->insertRow(new_row);
        m_model->setData(m_model->index(new_row,0),QVariant::fromValue(a));
    }
}
