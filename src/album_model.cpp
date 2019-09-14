#include "album_model.h"


AlbumModel::AlbumModel(QObject *parent) : QObject(parent)
{
    m_model = new QStandardItemModel(this);
    m_track_model = new QStandardItemModel(this);
    se = new SearchEngineCore::SearchEngine(this);

    connect(&AlbumCore::Album::getInstance(), &AlbumCore::Album::searchFinished, this, &AlbumModel::inputAlbumResults);
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

void AlbumModel::addTracksToMPD(int index)
{
    QStandardItem* item = nullptr;
    if (index == -1) {
        bool flag = true;
        int i = 0;
        QStringList cmd;
        cmd << "add";
        while (flag) {
            item = m_track_model->item(i, 0);
            if (item != nullptr) {
                Track t = item->data(Qt::DisplayRole).value<Track>();
                cmd.append(QString("tidal://track/") + t.id());
                i++;
            } else {
                flag = false;
            }
        }
        QProcess::execute("mpc", cmd);
    } else {

    }
}

void AlbumModel::inputAlbumResults()
{
    m_model->clear();
    m_model->insertColumn(0);
    m_model->insertColumn(1);
    for (const auto& album : AlbumCore::Album::getInstance().getAlbums()) {
        const int new_row= m_model->rowCount();
        const Album a(album.second.aid, album.second.title, album.second.cover, album.second.date, album.second.upc);
        m_model->insertRow(new_row);
        m_model->setData(m_model->index(new_row,0),QVariant::fromValue(a));
        m_model->setData(m_model->index(new_row,1),QVariant::fromValue(album.second.date));
    }
    m_model->sort(1, Qt::DescendingOrder);
    AlbumCore::Album::getInstance().clear();
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
