#include "album_model.h"

AlbumModel::AlbumModel(QObject *parent) : QObject(parent)
{
    m_model = new QStandardItemModel(this);
    m_track_model = new QStandardItemModel(this);
    se = new SearchEngineCore::SearchEngine(this);

    connect(se, &SearchEngineCore::SearchEngine::resultReady, this, &AlbumModel::inputAlbumResults);
    connect(se, &SearchEngineCore::SearchEngine::findInTidalReady, this, &AlbumModel::handleTrackResults);
}

QStandardItemModel *AlbumModel::model() const
{
    return m_model;
}

QStandardItemModel *AlbumModel::track_model() const
{
    return m_track_model;
}

void AlbumModel::search(QString keywords)
{
    se->search(keywords);
}

void AlbumModel::getTracks(QString upc, QString title)
{
    qDebug() << "getting!" << upc << title;
    if (title.size() > 20) {
        title.resize(20);
    }
    se->findInTidal(upc.toStdString(), title.toStdString());
}

void AlbumModel::inputAlbumResults(std::map<QString, SearchEngineCore::Album> albums)
{
    m_model->clear();
    m_model->insertColumn(0);
    for (auto album : albums) {
        const int new_row= m_model->rowCount();
        const Album a(album.second.aid, album.second.title, album.second.cover, album.second.date, album.second.upc);
        m_model->insertRow(new_row);
        m_model->setData(m_model->index(new_row,0),QVariant::fromValue(a));
    }
    emit dataFetched();
}

void AlbumModel::handleTrackResults(std::vector<TidalCore::Track> t, QString album_title, QString cover_large)
{
    m_track_model->clear();
    m_track_model->insertColumn(0);
    for (auto track : t) {
        const int new_row= m_track_model->rowCount();
        const Track t(QString::fromStdString(track.tid), QString::fromStdString(track.title), track.duration);
        m_track_model->insertRow(new_row);
        m_track_model->setData(m_track_model->index(new_row,0),QVariant::fromValue(t));
    }
    emit dataTrackFetched(album_title, cover_large);
}
