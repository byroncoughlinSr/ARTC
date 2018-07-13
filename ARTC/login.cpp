#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{
    username=ui->lineEdit_username->text();
    password=ui->lineEdit_password->text();
    close();
}

QString Login::getUsername()
{
    return username;
}

QString Login::getPassword()
{
    return password;
}
