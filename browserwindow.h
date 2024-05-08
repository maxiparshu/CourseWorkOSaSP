#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QProgressBar;
QT_END_NAMESPACE

class Browser;
class TabWidget;
class WebView;

//окно браузера
class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(Browser *browser, QWebEngineProfile *profile);

    // установка размера
    QSize sizeHint() const override;
    //вкладки
    TabWidget *tabWidget() const;
    //текущие вкладки
    WebView *currentTab() const;
    //для создания новых окон
    Browser *browser();
protected:
    //событие закрытия
    void closeEvent(QCloseEvent *event) override;

private slots:
    //обработчик открытия файла
    void handleFileOpenTriggered();
    //обработчик сохранения файла
    void handleFileSaveTriggered();
    //обработчик открытия окна
    void handleNewWindowTriggered();
    //обработчик поиска по страницу
    void handleFindActionTriggered();
    void handleShowWindowTriggered();
    //обработчик при загрузки страниц
    void handleWebViewLoadProgress(int);
    void handleWebViewTitleChanged(const QString &title);
    void handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void handleFindTextFinished(const QWebEngineFindTextResult &result);

private:
    //создание меню и панели
    QMenu *createFileMenu(TabWidget *tabWidget);
    QMenu *createEditMenu();
    QMenu *createViewMenu(QToolBar *toolBar);
    QMenu *createWindowMenu(TabWidget *tabWidget);
    QToolBar *createToolBar();

private:
    Browser *m_browser; // браузер
    TabWidget *m_tabWidget; // вкладки
    QProgressBar *m_progressBar = nullptr; // загрузка страницы
    QAction *m_historyBackAction = nullptr; // назад
    QAction *m_historyForwardAction = nullptr; // вперед
    QAction *m_stopAction = nullptr; // остановить
    QAction *m_reloadAction = nullptr; // презагрузить страницу
    QAction *m_stopReloadAction = nullptr;// конец перезагрузки
    QLineEdit *m_urlLineEdit = nullptr; // строка поиска
    QAction *m_favAction = nullptr; // картинка
    QString m_lastSearch; // последняя страница
};

#endif // BROWSERWINDOW_H
