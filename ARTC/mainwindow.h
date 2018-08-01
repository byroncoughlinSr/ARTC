#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glob.h"
#include "databasehelper.h"
#include "login.h"
#include "pedigree.h"

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
    DatabaseHelper databaseHelper = DatabaseHelper(login.getUsername(), login.getPassword());


private slots:
    void on_action_New_Host_triggered();


private:
    Ui::MainWindow *ui;
    QString *uname;
    QString *pword;
};

#endif // MAINWINDOW_H
