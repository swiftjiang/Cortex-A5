/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Mon Jul 3 15:36:19 2017
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QwtPlot *plot;
    QPushButton *clean_btn;
    QComboBox *showdata;
    QComboBox *comboBox_2;
    QLabel *label;
    QLabel *label_2;
    QComboBox *plot1;
    QComboBox *plot2;
    QComboBox *plot3;
    QComboBox *plot4;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QPushButton *paint;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 480);
        MainWindow->setMouseTracking(false);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        plot = new QwtPlot(centralWidget);
        plot->setObjectName(QString::fromUtf8("plot"));
        plot->setGeometry(QRect(0, 90, 581, 331));
        plot->setStyleSheet(QString::fromUtf8("gridline-color: rgb(255, 255, 255);"));
        plot->setFrameShadow(QFrame::Plain);
        plot->setAutoReplot(true);
        clean_btn = new QPushButton(centralWidget);
        clean_btn->setObjectName(QString::fromUtf8("clean_btn"));
        clean_btn->setGeometry(QRect(600, 360, 81, 31));
        showdata = new QComboBox(centralWidget);
        showdata->setObjectName(QString::fromUtf8("showdata"));
        showdata->setGeometry(QRect(60, 40, 81, 22));
        comboBox_2 = new QComboBox(centralWidget);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
        comboBox_2->setGeometry(QRect(200, 40, 71, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(60, 10, 81, 21));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(210, 10, 54, 21));
        plot1 = new QComboBox(centralWidget);
        plot1->setObjectName(QString::fromUtf8("plot1"));
        plot1->setGeometry(QRect(661, 81, 121, 20));
        plot2 = new QComboBox(centralWidget);
        plot2->setObjectName(QString::fromUtf8("plot2"));
        plot2->setGeometry(QRect(661, 147, 121, 20));
        plot3 = new QComboBox(centralWidget);
        plot3->setObjectName(QString::fromUtf8("plot3"));
        plot3->setGeometry(QRect(661, 213, 121, 20));
        plot4 = new QComboBox(centralWidget);
        plot4->setObjectName(QString::fromUtf8("plot4"));
        plot4->setGeometry(QRect(661, 279, 121, 20));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(590, 80, 54, 31));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(590, 140, 61, 41));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(590, 210, 54, 31));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(590, 270, 71, 41));
        paint = new QPushButton(centralWidget);
        paint->setObjectName(QString::fromUtf8("paint"));
        paint->setGeometry(QRect(710, 362, 75, 31));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        clean_btn->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\345\233\276\345\203\217", 0, QApplication::UnicodeUTF8));
        showdata->clear();
        showdata->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "\350\260\261\345\233\276", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "\346\265\213\351\207\217\347\273\223\346\236\234", 0, QApplication::UnicodeUTF8)
        );
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "CH1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "CH2", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:14pt;\">\346\230\276\347\244\272\346\225\260\346\215\256</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:14pt;\">\351\200\232\351\201\223</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:14pt;\">plot1</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:16pt;\">plot2</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:16pt;\">plot3</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:16pt;\">plot4</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        paint->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213\347\273\230\345\233\276", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
