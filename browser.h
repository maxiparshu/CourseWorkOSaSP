#ifndef BROWSER_H
#define BROWSER_H

#include "downloadmanager.h"
#include <QList>
#include <QWebEngineProfile>

class BrowserWindow;
// управляет работой окон
class Browser
{
public:
    Browser(); // конструктор


    BrowserWindow *createWindow(); // показывает и создает окно

    QList<BrowserWindow*> windows() { return m_windows; } // возвращает список окон
    DownloadManager &downloadManagerWidget() { return m_downloadManagerWidget; }
private:
    BrowserWindow *createHiddenWindow(); // создает окно
    QList<BrowserWindow*> m_windows; // список окон
    DownloadManager m_downloadManagerWidget; // менеджер загрузок
    QScopedPointer<QWebEngineProfile> profile; // профиль
};

#endif // BROWSER_H
