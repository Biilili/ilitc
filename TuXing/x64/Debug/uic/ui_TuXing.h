/********************************************************************************
** Form generated from reading UI file 'TuXing.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TUXING_H
#define UI_TUXING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TuXingClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TuXingClass)
    {
        if (TuXingClass->objectName().isEmpty())
            TuXingClass->setObjectName(QString::fromUtf8("TuXingClass"));
        TuXingClass->resize(600, 400);
        menuBar = new QMenuBar(TuXingClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        TuXingClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TuXingClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        TuXingClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(TuXingClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        TuXingClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(TuXingClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        TuXingClass->setStatusBar(statusBar);

        retranslateUi(TuXingClass);

        QMetaObject::connectSlotsByName(TuXingClass);
    } // setupUi

    void retranslateUi(QMainWindow *TuXingClass)
    {
        TuXingClass->setWindowTitle(QApplication::translate("TuXingClass", "TuXing", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TuXingClass: public Ui_TuXingClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TUXING_H
