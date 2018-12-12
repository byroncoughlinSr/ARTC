#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "databasehelper.h"
#include "pedigreetreepaint.h"
#include "login.h"
#include "pedigree.h"
#include <QGraphicsScene>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Login login;
     explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Pedigree pedigree;

    QSqlQueryModel *model=new QSqlQueryModel;

private slots:
    void on_action_New_Host_triggered();
    void on_individualListView_doubleClicked(const QModelIndex &index);

    void on_actionPedigree_triggered();

private:
    Ui::MainWindow *ui;
    pedigreeTreePaint *tree;
    QGraphicsView *view;
    QString *uname;
    QString *pword;
    QGraphicsScene *scene;
    QGraphicsRectItem *rect;
};

#endif // MAINWINDOW_H
