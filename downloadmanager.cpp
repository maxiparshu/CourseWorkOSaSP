#include "downloadmanager.h"

#include "browser.h"
#include "browserwindow.h"
#include "downloadwidget.h"

#include <QFileDialog>
#include <QDir>
#include <QWebEngineDownloadRequest>

DownloadManager::DownloadManager(QWidget *parent)
    : QWidget(parent)
    , m_numDownloads(0)
{
    setupUi(this);
}
void DownloadManager::downloadRequested(QWebEngineDownloadRequest *download)
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save as"), QDir(download->downloadDirectory()).filePath(download->downloadFileName()));
    if (path.isEmpty())
        return;
    download->setDownloadDirectory(QFileInfo(path).path());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
    add(new DownloadWidget(download));
    show();
}

void DownloadManager::add(DownloadWidget *downloadWidget)
{
    connect(downloadWidget, &DownloadWidget::removeClicked, this, &DownloadManager::remove);
    // добавляем виджет загрузки
    itemsLayout->insertWidget(0, downloadWidget, 0, Qt::AlignTop);
    if (m_numDownloads++ == 0)
        zeroItemsLabel->hide();

}

void DownloadManager::remove(DownloadWidget *downloadWidget)
{
    itemsLayout->removeWidget(downloadWidget);
    downloadWidget->deleteLater();
    if (--m_numDownloads == 0)
        zeroItemsLabel->show();
}
