#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QWebEngineFindTextResult>
#include <QWebEnginePage>
#include <QStack>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

class WebView;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWebEngineProfile *profile, QWidget *parent = nullptr);

    WebView *currentWebView() const;

signals:
    // current tab/page signals
    void linkHovered(const QString &link); // наведено на ссылку
    void loadProgress(int progress); // загрузка страницы
    void titleChanged(const QString &title); // изменено название
    void favIconChanged(const QIcon &icon); // измение иконки
    void urlChanged(const QUrl &url); // изменение ссылки
    void webActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled); // отключиние действие
    void findTextFinished(const QWebEngineFindTextResult &result); // поиск текска

public slots:
    void setUrl(const QUrl &url);
    void triggerWebPageAction(QWebEnginePage::WebAction action);
    WebView *createTab();
    WebView *createBackgroundTab();
    void closeTab(int index);
    void nextTab();
    void previousTab();
    void openLastClosed();

private slots:
    void handleCurrentChanged(int index);
    void handleContextMenuRequested(const QPoint &pos);
    void createTabWithHtml(int index);
    void cloneTab(int index);
    void closeOtherTabs(int index);
    void reloadAllTabs();
    void reloadTab(int index);
private:
    WebView *webView(int index) const;
    void setupView(WebView *webView);
    QStack<QPair<int, WebView*>> lastClosed;
    QWebEngineProfile *m_profile;
};

#endif // TABWIDGET_H
