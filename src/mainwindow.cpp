#include <QVBoxLayout>
#include <QQmlContext>
#include <QQuickItem>
#include "search_engine.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->resize(QSize(1360, 800));
    album_model = new AlbumModel(this);

    root_qml = new QQuickWidget(this);
    root_qml->rootContext()->setContextProperty("album_model", album_model);
    root_qml->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    root_qml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QQuickItem* header = root_qml->rootObject()->findChild<QQuickItem *>(QStringLiteral("header"));
    QQuickItem* gv = root_qml->rootObject()->findChild<QQuickItem *>(QStringLiteral("grid_view"));

    QObject::connect(header, SIGNAL(search_clicked(QString)), album_model, SLOT(search(QString)));
    QObject::connect(gv, SIGNAL(album_clicked(QString, QString)), album_model, SLOT(getTracks(QString, QString)));
    QObject::connect(album_model, SIGNAL(dataFetched()), gv, SLOT(unsetBusy()));
    QObject::connect(album_model, SIGNAL(dataTrackFetched(QVariant, QVariant)), gv, SLOT(handleTracksData(QVariant, QVariant)));

    setCentralWidget(root_qml);
}

MainWindow::~MainWindow()
{

}
