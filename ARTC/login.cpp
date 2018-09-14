#include "login.h"
#include "ui_login.h"
#include "databasehelper.h"

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
    DatabaseHelper *dbHelper = DatabaseHelper::getInstance();

    username=ui->lineEdit_username->text();
    password=ui->lineEdit_password->text();

    dbHelper->setUserId(username);
    dbHelper->setPassword(password);

    close();
}

