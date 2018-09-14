#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QDebug>
#include "personlistdialog.h"
#include "databasehelper.h"
#include "ui_personlistdialog.h"

PersonListDialog::PersonListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PersonListDialog)
{
    DatabaseHelper *databaseHelper = DatabaseHelper::getInstance();
    model=databaseHelper->getListView();

    ui->setupUi(this);
    ui->personListView->setModel(model);
}

PersonListDialog::~PersonListDialog()
{
    delete ui;
}



