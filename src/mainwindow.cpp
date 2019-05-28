#include <QVBoxLayout>
#include <QQmlContext>
#include <QQuickItem>
#include "spotify/spotify_core.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *vb;
    vb = new QVBoxLayout(this);
    album_model = new AlbumModel(this);

    root_qml = new QQuickWidget(this);
    root_qml->rootContext()->setContextProperty("album_model", album_model);
    root_qml->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    root_qml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QQuickItem* header = root_qml->rootObject()->findChild<QQuickItem *>(QStringLiteral("header"));

    QObject::connect(header, SIGNAL(search_clicked(QString)), album_model, SLOT(search(QString)));

    vb->addWidget(root_qml);
    vb->setMargin(0);
    setLayout(vb);

    SpotifyCore::Spotify* sp = new SpotifyCore::Spotify(this);
    sp->search("takashi yoshimatsu");
}

MainWindow::~MainWindow()
{

}
