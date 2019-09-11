#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "databasehelper.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT
private:
    Ui::Login *ui;
    QString username, password;

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    QString getUsername();
    QString getPassword();
    DatabaseHelper databaseHelper = DatabaseHelper(username, password);

private slots:
    void on_pushButton_clicked();
};

#endif // LOGIN_H
