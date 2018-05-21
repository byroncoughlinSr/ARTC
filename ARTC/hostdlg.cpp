#include "hostdlg.h"
#include "ui_hostdlg.h"
#include "databasehelper.h"
#include <string>
#include <QDate>
#include "person.h"

Hostdlg::Hostdlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Hostdlg)
{
    ui->setupUi(this);
}

Hostdlg::~Hostdlg()
{
    delete ui;
}

void Hostdlg::on_buttonBox_accepted()
{
    int hostId;
    DatabaseHelper databaseHelper;
    databaseHelper.createConnection();

    host.firstName = ui->firstNameTxt->toPlainText().toStdString();
    host.middleName = ui->middleNameTxt->toPlainText().toStdString();;
    host.lastName = ui->lastNameTxt->toPlainText().toStdString();
    host.birthdate = ui->dateEdit->date();

    if (ui->maleBtn->isChecked())
    {
        host.sex = 'M';
    }
    else
    {
        host.sex = 'F';
    }
    databaseHelper.addPerson(host);
    hostId = databaseHelper.getPersonId(host);
}
