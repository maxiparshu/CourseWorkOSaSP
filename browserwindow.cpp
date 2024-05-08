#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>

using namespace Qt::StringLiterals;

BrowserWindow::BrowserWindow(Browser *browser,QWebEngineProfile *profile)
    : m_browser(browser)
    , m_tabWidget(new TabWidget(profile,this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    //чтобы при нажатие переключался фокус на окно
    setFocusPolicy(Qt::ClickFocus);
    m_progressBar = new QProgressBar(this);
    QToolBar *toolbar = createToolBar();
    addToolBar(toolbar);
    // добавляем панели для работы
    menuBar()->addMenu(createFileMenu(m_tabWidget));
    menuBar()->addMenu(createEditMenu());
    menuBar()->addMenu(createViewMenu(toolbar));
    menuBar()->addMenu(createWindowMenu(m_tabWidget));
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    //отделяем
    addToolBarBreak();
    m_progressBar->setMaximumHeight(1);
    //убираем техкт на шкале
    m_progressBar->setTextVisible(false);
    m_progressBar->setStyleSheet(u"QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"_s);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    //изменить название окна и  вкладки
    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    //показать в статус баре ссылка
    connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
        statusBar()->showMessage(url);
    });
    //для загрузки
    connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
    //изменять состояние кнопок
    connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);

    connect(m_tabWidget, &TabWidget::urlChanged, [this](const QUrl &url) {
        m_urlLineEdit->setText(url.toDisplayString());
    });

    connect(m_urlLineEdit, &QLineEdit::returnPressed, [this]() {
        m_tabWidget->setUrl(QUrl::fromUserInput(m_urlLineEdit->text()));
    });

    connect(m_tabWidget, &TabWidget::findTextFinished, this, &BrowserWindow::handleFindTextFinished);
    //изменить иконку
    connect(m_tabWidget, &TabWidget::favIconChanged, m_favAction, &QAction::setIcon);
    QAction *focusUrlLineEditAction = new QAction(this);
    addAction(focusUrlLineEditAction);
    focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () {
        m_urlLineEdit->setFocus(Qt::ShortcutFocusReason);
    });
    handleWebViewTitleChanged("Google");
    m_tabWidget->createTab();
}

QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::primaryScreen()->geometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}
Browser *BrowserWindow::browser() { return m_browser; }

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

void BrowserWindow::handleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QDir::homePath(),
                                           tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz)"));
    if (url.isEmpty())
        return;
    currentTab()->setUrl(url);
}

void BrowserWindow::handleFileSaveTriggered()
{
    auto url = currentTab()->title().toUtf8();
    QString filename = QFileDialog::getExistingDirectory(this,
                                                    tr("Save " + url),
                                                         QDir::homePath());
    filename +="/" + url + ".html";

    currentTab()->page()->toHtml([filename](const QString &html){
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            QTextStream stream(&file);
            stream << html;
            file.close();
        }
    });


}

QMenu *BrowserWindow::createFileMenu(TabWidget *tabWidget)
{
    QMenu *fileMenu = new QMenu(tr("&File"));
    fileMenu->addAction(tr("&New Window"), QKeySequence::New, this, &BrowserWindow::handleNewWindowTriggered);

    QAction *newTabAction = new QAction(tr("New &Tab"), this);

    newTabAction->setShortcut(QKeySequence::AddTab);
    //создание новой вкладки
    connect(newTabAction, &QAction::triggered, this, [this]() {
        m_tabWidget->createTab();
        m_urlLineEdit->setFocus();
    });
    fileMenu->addAction(newTabAction);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Open File..."), QKeySequence::Open, this, &BrowserWindow::handleFileOpenTriggered);
    fileMenu->addAction(tr("&Save ..."), QKeySequence::Save, this, &BrowserWindow::handleFileSaveTriggered);
    fileMenu->addSeparator();
    QAction *closeTabAction = new QAction(tr("&Close Tab"), this);
    closeTabAction->setShortcut(QKeySequence::Close);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->closeTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);
    QAction *returnTabAction = new QAction(tr("&Return Tab"), this);
    returnTabAction->setShortcuts(QKeySequence::Redo);
    connect(returnTabAction, &QAction::triggered,[tabWidget]() {
        tabWidget->openLastClosed();
    });
    fileMenu->addAction(returnTabAction);

    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    //при открытии менять название
    connect(fileMenu, &QMenu::aboutToShow, [this, closeAction]() {
        if (m_browser->windows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    return fileMenu;
}

QMenu *BrowserWindow::createEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"));

    QAction *findAction = editMenu->addAction(tr("&Find"));
    findAction->setShortcuts(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &BrowserWindow::handleFindActionTriggered);

    QAction *findNextAction = editMenu->addAction(tr("Find &Next"));
    findNextAction->setShortcut(QKeySequence::FindNext);
    connect(findNextAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch);
    });

    QAction *findPreviousAction = editMenu->addAction(tr("Find &Previous"));
    findPreviousAction->setShortcut(QKeySequence::FindPrevious);
    connect(findPreviousAction, &QAction::triggered, [this]() {
        if (!currentTab() || m_lastSearch.isEmpty())
            return;
        currentTab()->findText(m_lastSearch, QWebEnginePage::FindBackward);
    });

    return editMenu;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    });

    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    });

    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(1.0);
    });


    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+H"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);
    return viewMenu;
}

QMenu *BrowserWindow::createWindowMenu(TabWidget *tabWidget)
{
    QMenu *menu = new QMenu(tr("&Window"));

    QAction *nextTabAction = new QAction(tr("Show Next Tab"), this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    nextTabAction->setShortcuts(shortcuts);
    connect(nextTabAction, &QAction::triggered, tabWidget, &TabWidget::nextTab);

    QAction *previousTabAction = new QAction(tr("Show Previous Tab"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    previousTabAction->setShortcuts(shortcuts);
    connect(previousTabAction, &QAction::triggered, tabWidget, &TabWidget::previousTab);

    menu->clear();
    menu->addAction(nextTabAction);
    menu->addAction(previousTabAction);
    return menu;
}


static bool isBackspace(const QKeySequence &k)
{
    return (k[0].key() & Qt::Key_unknown) == Qt::Key_Backspace;
}

static QList<QKeySequence> removeBackspace(QList<QKeySequence> keys)
{
    const auto it = std::find_if(keys.begin(), keys.end(), isBackspace);
    if (it != keys.end())
        keys.erase(it);
    return keys;
}

QToolBar *BrowserWindow::createToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    //чтобы не двигался
    navigationBar->setMovable(false);
    //нельзя изменить прозрачность тул бара
    navigationBar->toggleViewAction()->setEnabled(false);

    // действие для перехода назад
    m_historyBackAction = new QAction(this);
    auto backShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Back));
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    m_historyBackAction->setShortcuts(backShortcuts);//сочетания клавиш
    m_historyBackAction->setIconVisibleInMenu(false);
    m_historyBackAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious,
                                                  QIcon(":go-previous.png"_L1))); // иконка
    m_historyBackAction->setToolTip(tr("Go back in history"));
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });//сигнал для перехода назад
    navigationBar->addAction(m_historyBackAction);

    //переход вперед
    m_historyForwardAction = new QAction(this);
    auto fwdShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Forward));
    fwdShortcuts.append(QKeySequence(Qt::Key_Forward));
    m_historyForwardAction->setShortcuts(fwdShortcuts);
    m_historyForwardAction->setIconVisibleInMenu(false);
    m_historyForwardAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext,
                                                     QIcon(":go-next.png"_L1)));
    m_historyForwardAction->setToolTip(tr("Go forward in history"));
    connect(m_historyForwardAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(m_historyForwardAction);

    m_stopReloadAction = new QAction(this);
    // по нажатию перестаем обновлять
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    m_urlLineEdit = new QLineEdit(this);
    m_favAction = new QAction(this); // иконка слева
    m_urlLineEdit->addAction(m_favAction, QLineEdit::LeadingPosition);
    navigationBar->addWidget(m_urlLineEdit);

    //действие с менеждером скачиваний
    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(u":go-bottom.png"_s));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered,
            &m_browser->downloadManagerWidget(), &QWidget::show); // по нажатию открывать менеджер скачиваний

    return navigationBar;
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    // активно или не активно состояние
    switch (action) {
    case QWebEnginePage::Back:
        m_historyBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        m_historyForwardAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        m_reloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        m_stopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}


void BrowserWindow::handleNewWindowTriggered()
{
    BrowserWindow *window = m_browser->createWindow();
    window->m_urlLineEdit->setFocus();
}


void BrowserWindow::handleFindActionTriggered()
{
    if (!currentTab())
        return;
    bool ok = false;
    QString search = QInputDialog::getText(this, tr("Find"),
                                           tr("Find:"), QLineEdit::Normal,
                                           m_lastSearch, &ok);
    if (ok) {
        m_lastSearch = search;
        currentTab()->findText(m_lastSearch);
    }
}


TabWidget *BrowserWindow::tabWidget() const
{
    return m_tabWidget;
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon = QIcon::fromTheme(QIcon::ThemeIcon::ProcessStop,
                                             QIcon(":process-stop.png"_L1));
    static QIcon reloadIcon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh,
                                               QIcon(":view-refresh.png"_L1));

    if (0 < progress && progress < 100) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_progressBar->setValue(progress);
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_progressBar->setValue(0);
    }
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    setWindowTitle(title + " - ParshutoBrowser");
}

void BrowserWindow::handleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QList<BrowserWindow*> windows = m_browser->windows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->currentTab()->setFocus();
    }
}

void BrowserWindow::handleFindTextFinished(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0) {
        statusBar()->showMessage(tr("\"%1\" not found.").arg(m_lastSearch));
    } else {
        statusBar()->showMessage(tr("\"%1\" found: %2/%3").arg(m_lastSearch,
                                                               QString::number(result.activeMatch()),
                                                               QString::number(result.numberOfMatches())));
    }
}

