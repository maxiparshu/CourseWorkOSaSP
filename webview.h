#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QIcon>
#include <QWebEngineView>
#include <QWebEngineCertificateError>
#include <QWebEngineFileSystemAccessRequest>
#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineWebAuthUxRequest>

class WebPage;

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = nullptr);
    void setPage(WebPage *page);
    int loadProgress() const;

    QIcon favIcon() const;
    bool isWebActionEnabled(QWebEnginePage::WebAction webAction) const;
signals:
    void favIconChanged(const QIcon &icon);
    void webActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

private slots:
    void handleFeaturePermissionRequested(const QUrl &securityOrigin,
                                          QWebEnginePage::Feature feature);

private:
    void createWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);

private:
    int m_loadProgress = 100;
};

#endif // WEBVIEW_H
