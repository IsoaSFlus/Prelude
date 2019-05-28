#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QWidget>
#include <QQuickWidget>

#include "album_model.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:

public slots:

private:
    QQuickWidget* root_qml = nullptr;
    AlbumModel* album_model = nullptr;
};

#endif // MAINWINDOW_H
