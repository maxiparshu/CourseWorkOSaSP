#include "browser.h"
#include "browserwindow.h"
#include "tabwidget.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Browser browser;
    browser.createWindow();
    return a.exec();
}
