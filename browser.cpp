// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "browser.h"
#include "browserwindow.h"

#include <QWebEngineSettings>

using namespace Qt::StringLiterals;

Browser::Browser()
{
    // чтобы не заверщать работу если  закрыто
    m_downloadManagerWidget.setAttribute(Qt::WA_QuitOnClose, false);
}


BrowserWindow *Browser::createHiddenWindow()
{
    const QString name = u"Parshuto browser."_s + QLatin1StringView(qWebEngineChromiumVersion());
    //устанавливаем профиль
    profile.reset(new QWebEngineProfile(name));
    //разрешаем плагины
    profile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    //ускоряет загрузку страниц
    profile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
    profile->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
    //сигнал скачки с веб страниц
    QObject::connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
                     &m_downloadManagerWidget, &DownloadManager::downloadRequested);
    auto mainWindow = new BrowserWindow(this,QWebEngineProfile::defaultProfile());
    m_windows.append(mainWindow);
    //если закрывается окно то удалить окно
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        this->m_windows.removeOne(mainWindow);
    });
    return mainWindow;
}

BrowserWindow *Browser::createWindow()
{
    auto *mainWindow = createHiddenWindow();
    mainWindow->show();
    return mainWindow;
}
