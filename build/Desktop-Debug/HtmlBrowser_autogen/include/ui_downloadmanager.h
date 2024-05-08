/********************************************************************************
** Form generated from reading UI file 'downloadmanager.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADMANAGER_H
#define UI_DOWNLOADMANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DownloadManagerWidget
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *items;
    QVBoxLayout *itemsLayout;
    QLabel *zeroItemsLabel;

    void setupUi(QWidget *DownloadManagerWidget)
    {
        if (DownloadManagerWidget->objectName().isEmpty())
            DownloadManagerWidget->setObjectName("DownloadManagerWidget");
        DownloadManagerWidget->resize(400, 251);
        verticalLayout = new QVBoxLayout(DownloadManagerWidget);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(DownloadManagerWidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        items = new QWidget();
        items->setObjectName("items");
        items->setEnabled(true);
        items->setGeometry(QRect(0, 0, 380, 231));
        items->setLayoutDirection(Qt::LeftToRight);
        items->setStyleSheet(QString::fromUtf8(""));
        itemsLayout = new QVBoxLayout(items);
        itemsLayout->setSpacing(3);
        itemsLayout->setObjectName("itemsLayout");
        itemsLayout->setContentsMargins(3, 3, 3, 3);
        zeroItemsLabel = new QLabel(items);
        zeroItemsLabel->setObjectName("zeroItemsLabel");
        zeroItemsLabel->setFrameShape(QFrame::StyledPanel);
        zeroItemsLabel->setTextFormat(Qt::AutoText);
        zeroItemsLabel->setScaledContents(false);

        itemsLayout->addWidget(zeroItemsLabel);

        scrollArea->setWidget(items);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(DownloadManagerWidget);

        QMetaObject::connectSlotsByName(DownloadManagerWidget);
    } // setupUi

    void retranslateUi(QWidget *DownloadManagerWidget)
    {
        DownloadManagerWidget->setWindowTitle(QCoreApplication::translate("DownloadManagerWidget", "Downloads", nullptr));
        zeroItemsLabel->setText(QCoreApplication::translate("DownloadManagerWidget", "Zero downloads", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DownloadManagerWidget: public Ui_DownloadManagerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DOWNLOADMANAGER_H
