#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "webpage.h"
#include "webpopupwindow.h"
#include "webview.h"
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QAuthenticator>
#include <QTimer>
#include <QStyle>

using namespace Qt::StringLiterals;

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::loadStarted, [this]() {
        m_loadProgress = 0;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::loadProgress, [this](int progress) {
        m_loadProgress = progress;
    });
    connect(this, &QWebEngineView::loadFinished, [this](bool success) {
        m_loadProgress = success ? 100 : -1;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::iconChanged, [this](const QIcon &) {
        emit favIconChanged(favIcon());
    });

}

inline QString questionForFeature(QWebEnginePage::Feature feature)
{
    switch (feature) {
    case QWebEnginePage::Geolocation:
        return QObject::tr("Allow %1 to access your location information?");
    case QWebEnginePage::MediaAudioCapture:
        return QObject::tr("Allow %1 to access your microphone?");
    case QWebEnginePage::MediaVideoCapture:
        return QObject::tr("Allow %1 to access your webcam?");
    case QWebEnginePage::MediaAudioVideoCapture:
        return QObject::tr("Allow %1 to access your microphone and webcam?");
    case QWebEnginePage::MouseLock:
        return QObject::tr("Allow %1 to lock your mouse cursor?");
    case QWebEnginePage::DesktopVideoCapture:
        return QObject::tr("Allow %1 to capture video of your desktop?");
    case QWebEnginePage::DesktopAudioVideoCapture:
        return QObject::tr("Allow %1 to capture audio and video of your desktop?");
    case QWebEnginePage::Notifications:
        return QObject::tr("Allow %1 to show notification on your desktop?");
    }
    return QString();
}

void WebView::setPage(WebPage *page)
{
    if (auto oldPage = qobject_cast<WebPage *>(QWebEngineView::page())) {
        disconnect(oldPage, &QWebEnginePage::featurePermissionRequested, this,
                   &WebView::handleFeaturePermissionRequested);
    }
    createWebActionTrigger(page,QWebEnginePage::Forward);
    createWebActionTrigger(page,QWebEnginePage::Back);
    createWebActionTrigger(page,QWebEnginePage::Reload);
    createWebActionTrigger(page,QWebEnginePage::Stop);
    QWebEngineView::setPage(page);

    connect(page, &QWebEnginePage::featurePermissionRequested, this,
            &WebView::handleFeaturePermissionRequested);
}

int WebView::loadProgress() const
{
    return m_loadProgress;
}

QIcon WebView::favIcon() const
{
    QIcon favIcon = icon();
    if (!favIcon.isNull())
        return favIcon;

    if (m_loadProgress < 0) {
        static QIcon errorIcon(u":dialog-error.png"_s);
        return errorIcon;
    }
    if (m_loadProgress < 100) {
        static QIcon loadingIcon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh,
                                                    QIcon(":view-refresh.png"_L1));
        return loadingIcon;
    }

    static QIcon defaultIcon(u":text-html.png"_s);
    return defaultIcon;
}

void WebView::createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction]{
        emit webActionEnabledChanged(webAction, action->isEnabled());
    });
}

bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    BrowserWindow *mainWindow = qobject_cast<BrowserWindow*>(window());
    if (!mainWindow)
        return nullptr;

    switch (type) {
    case QWebEnginePage::WebBrowserTab: {
        return mainWindow->tabWidget()->createTab();
    }
    case QWebEnginePage::WebBrowserBackgroundTab: {
        return mainWindow->tabWidget()->createBackgroundTab();
    }
    case QWebEnginePage::WebBrowserWindow: {
        return mainWindow->browser()->createWindow()->currentTab();
    }
    case QWebEnginePage::WebDialog: {
        WebPopupWindow *popup = new WebPopupWindow(page()->profile());
        return popup->view();
    }
    }
    return nullptr;
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    const QList<QAction *> actions = menu->actions();
    menu->popup(event->globalPos());
}

void WebView::handleFeaturePermissionRequested(const QUrl &securityOrigin,
                                               QWebEnginePage::Feature feature)
{
    QString title = tr("Permission Request");
    QString question = questionForFeature(feature).arg(securityOrigin.host());
    if (!question.isEmpty() && QMessageBox::question(window(), title, question) == QMessageBox::Yes)
        page()->setFeaturePermission(securityOrigin, feature,
                                     QWebEnginePage::PermissionGrantedByUser);
    else
        page()->setFeaturePermission(securityOrigin, feature,
                                     QWebEnginePage::PermissionDeniedByUser);
}
