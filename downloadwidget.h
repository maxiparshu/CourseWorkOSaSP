#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include "ui_downloadwidget.h"

#include <QFrame>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

// виджет загрузки
class DownloadWidget : public QFrame, public Ui::DownloadWidget
{
    Q_OBJECT
public:
    explicit DownloadWidget(QWebEngineDownloadRequest *download, QWidget *parent = nullptr);

signals:
    void removeClicked(DownloadWidget *self);

private slots:
    //обновить шкалу
    void updateWidget();

private:
    QString withUnit(qreal bytes);

    QWebEngineDownloadRequest *m_download;
    QElapsedTimer m_timeAdded;


};

#endif // DOWNLOADWIDGET_H
