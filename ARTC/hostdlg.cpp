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

    host.firstName = ui->firstNameTxt->toPlainText();
    host.middleName = ui->middleNameTxt->toPlainText();
    host.lastName = ui->lastNameTxt->toPlainText();
    host.birthdate = ui->dateEdit->date();

    if (ui->maleBtn->isChecked())
    {
        host.sex = 'M';
    }
    else
    {
        host.sex = 'F';
    }

}

Person::Individual Hostdlg::getHost()
{
    return host;
}
