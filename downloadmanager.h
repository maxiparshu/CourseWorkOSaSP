#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "ui_downloadmanager.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE


class DownloadWidget;
// класс журнала загрузок
class DownloadManager : public QWidget, Ui::DownloadManagerWidget
{
    Q_OBJECT
public:
    explicit DownloadManager(QWidget *parent = nullptr);
    void downloadRequested(QWebEngineDownloadRequest *webItem);

private:
    void add(DownloadWidget *downloadWidget);
    void remove(DownloadWidget *downloadWidget);

    int m_numDownloads;


};

#endif // DOWNLOADMANAGER_H
