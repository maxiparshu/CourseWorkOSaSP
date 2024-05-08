#include "downloadwidget.h"


#include <QFileInfo>
#include <QUrl>
#include <QWebEngineDownloadRequest>

using namespace Qt::StringLiterals;

DownloadWidget::DownloadWidget(QWebEngineDownloadRequest *download, QWidget *parent)
    : QFrame(parent)
    , m_download(download)
    , m_timeAdded()
{

    m_timeAdded.start();
    Ui::DownloadWidget ui;
    setupUi(this);
    dstName->setText(m_download->downloadFileName());
    srcUrl->setText(m_download->url().toDisplayString());
    connect(cancelButton, &QPushButton::clicked,
            [this](bool) {
                if (m_download->state() == QWebEngineDownloadRequest::DownloadInProgress)
                    m_download->cancel();
                else
                    emit removeClicked(this);
            });
    connect(m_download, &QWebEngineDownloadRequest::totalBytesChanged, this, &DownloadWidget::updateWidget);
    connect(m_download, &QWebEngineDownloadRequest::receivedBytesChanged, this, &DownloadWidget::updateWidget);

    connect(m_download, &QWebEngineDownloadRequest::stateChanged,this, &DownloadWidget::updateWidget);
    updateWidget();
}
inline QString DownloadWidget::withUnit(qreal bytes)
{
    if (bytes < (1 << 10))
        return tr("%L1 B").arg(bytes);
    if (bytes < (1 << 20))
        return tr("%L1 KiB").arg(bytes / (1 << 10), 0, 'f',2);
    if (bytes < (1 << 30))
        return tr("%L1 MiB").arg(bytes / (1 << 20), 0, 'f',2);
    return tr("%L1 GiB").arg(bytes / (1 << 30), 0, 'f',2);
}
void DownloadWidget::updateWidget()
{
    qreal totalBytes = m_download->totalBytes();
    qreal receivedBytes = m_download->receivedBytes();
    qreal bytesPerSecond = receivedBytes / m_timeAdded.elapsed() * 1000;

    auto state = m_download->state();
    switch (state) {
    case QWebEngineDownloadRequest::DownloadRequested: {
        break;
    }
    case QWebEngineDownloadRequest::DownloadInProgress:{
        if (totalBytes >= 0) {
            progressBar->setValue(qRound(100 * receivedBytes / totalBytes));
            progressBar->setDisabled(false);
            progressBar->setFormat(
                tr("%p% - %1 of %2 downloaded - %3/s")
                    .arg(withUnit(receivedBytes), withUnit(totalBytes),
                         withUnit(bytesPerSecond)));
        } else {
            progressBar->setValue(0);
            progressBar->setDisabled(false);
            progressBar->setFormat(
                tr("unknown size - %1 downloaded - %2/s")
                    .arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
        }
        break;
    }
    case QWebEngineDownloadRequest::DownloadCompleted:{
        progressBar->setValue(100);
        progressBar->setDisabled(true);
        progressBar->setFormat(
            tr("completed - %1 downloaded - %2/s")
                .arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
        break;
    }
    case QWebEngineDownloadRequest::DownloadCancelled: {
        progressBar->setValue(0);
        progressBar->setDisabled(true);
        progressBar->setFormat(
            tr("cancelled - %1 downloaded - %2/s")
                .arg(withUnit(receivedBytes), withUnit(bytesPerSecond)));
        break;
    }
    case QWebEngineDownloadRequest::DownloadInterrupted: {
        progressBar->setValue(0);
        progressBar->setDisabled(true);
        progressBar->setFormat(
            tr("interrupted: %1")
                .arg(m_download->interruptReasonString()));
        break;
    }
    }

    if (state == QWebEngineDownloadRequest::DownloadInProgress) {
         QIcon cancelIcon(QIcon::fromTheme(QIcon::ThemeIcon::ProcessStop,
                                                 QIcon(":process-stop.png")));
        cancelButton->setIcon(cancelIcon);
        cancelButton->setToolTip(tr("Stop downloading"));
    } else {
        QIcon removeIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear));
        cancelButton->setIcon(removeIcon);
        cancelButton->setToolTip(tr("Remove from list"));
    }
}
