#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineCertificateError>
#include <QWebEngineDesktopMediaRequest>



//страница
class WebPage : public QWebEnginePage
{
    Q_OBJECT
public:
   explicit WebPage(QWebEngineProfile *profile, QObject *parent = nullptr);
};

#endif // WEBPAGE_H
