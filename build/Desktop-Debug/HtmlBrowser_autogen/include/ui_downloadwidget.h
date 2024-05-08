/********************************************************************************
** Form generated from reading UI file 'downloadwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADWIDGET_H
#define UI_DOWNLOADWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DownloadWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *dstName;
    QPushButton *cancelButton;
    QLabel *srcUrl;
    QProgressBar *progressBar;

    void setupUi(QFrame *DownloadWidget)
    {
        if (DownloadWidget->objectName().isEmpty())
            DownloadWidget->setObjectName("DownloadWidget");
        DownloadWidget->resize(180, 124);
        gridLayout = new QGridLayout(DownloadWidget);
        gridLayout->setObjectName("gridLayout");
        dstName = new QLabel(DownloadWidget);
        dstName->setObjectName("dstName");

        gridLayout->addWidget(dstName, 0, 0, 1, 1);

        cancelButton = new QPushButton(DownloadWidget);
        cancelButton->setObjectName("cancelButton");

        gridLayout->addWidget(cancelButton, 0, 1, 2, 1);

        srcUrl = new QLabel(DownloadWidget);
        srcUrl->setObjectName("srcUrl");

        gridLayout->addWidget(srcUrl, 1, 0, 1, 1);

        progressBar = new QProgressBar(DownloadWidget);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(24);

        gridLayout->addWidget(progressBar, 2, 0, 1, 2);


        retranslateUi(DownloadWidget);

        QMetaObject::connectSlotsByName(DownloadWidget);
    } // setupUi

    void retranslateUi(QFrame *DownloadWidget)
    {
        DownloadWidget->setWindowTitle(QCoreApplication::translate("DownloadWidget", "Frame", nullptr));
        dstName->setText(QCoreApplication::translate("DownloadWidget", "TextLabel", nullptr));
        cancelButton->setText(QString());
        srcUrl->setText(QCoreApplication::translate("DownloadWidget", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloadWidget: public Ui_DownloadWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADWIDGET_H
