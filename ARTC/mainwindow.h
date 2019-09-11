#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasehelper.h"
#include "login.h"
#include "person.h"

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
   DatabaseHelper databaseHelper = DatabaseHelper(login.getUsername(), login.getPassword());
   Person::Individual person;

private slots:
    void on_action_New_Host_triggered();


private:
    Ui::MainWindow *ui;
    QString *uname;
    QString *pword;
    Person::Individual father;
    Person::Individual mother;
};

#endif // MAINWINDOW_H
