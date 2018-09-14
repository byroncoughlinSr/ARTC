/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_New_Host;
    QAction *actionPedigree;
    QWidget *centralWidget;
    QGraphicsView *graphicsView;
    QListView *individualListView;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1120, 656);
        action_New_Host = new QAction(MainWindow);
        action_New_Host->setObjectName(QString::fromUtf8("action_New_Host"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/rec/images/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_New_Host->setIcon(icon);
        actionPedigree = new QAction(MainWindow);
        actionPedigree->setObjectName(QString::fromUtf8("actionPedigree"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/rec/images/pedigree.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPedigree->setIcon(icon1);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        graphicsView = new QGraphicsView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(240, 0, 881, 561));
        individualListView = new QListView(centralWidget);
        individualListView->setObjectName(QString::fromUtf8("individualListView"));
        individualListView->setGeometry(QRect(0, 1, 241, 561));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1120, 30));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menuView->menuAction());
        menu_File->addAction(action_New_Host);
        menuView->addAction(actionPedigree);
        mainToolBar->addAction(action_New_Host);
        mainToolBar->addAction(actionPedigree);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "ARTC", 0, QApplication::UnicodeUTF8));
        action_New_Host->setText(QApplication::translate("MainWindow", "&New Host", 0, QApplication::UnicodeUTF8));
        actionPedigree->setText(QApplication::translate("MainWindow", "&Pedigree", 0, QApplication::UnicodeUTF8));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "&View", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
