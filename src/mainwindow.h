#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QWidget>
#include <QQuickWidget>

#include "album_model.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual QSize sizeHint() const { return QSize(1920, 800); }
signals:

public slots:

private:
    QQuickWidget* root_qml = nullptr;
    AlbumModel* album_model = nullptr;
};

#endif // MAINWINDOW_H
