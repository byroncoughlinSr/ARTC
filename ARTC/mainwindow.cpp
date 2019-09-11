#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hostdlg.h"
#include "login.h"
#include "databasehelper.h"
#include "pedigree.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Login login;

    //Run login dialog
    login.setModal(true);
    login.exec();

    databaseHelper = DatabaseHelper(login.getUsername(), login.getPassword());
    databaseHelper.createConnection();

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_Host_triggered()
{
    int fatherId;
    int motherId;
    Hostdlg hostdlg;
    Pedigree pedigree;
    Person::Individual father;
    Person::Individual mother;

    hostdlg.setModal(true);
    hostdlg.exec();
    person = hostdlg.getHost();

    //Add root person to database
    databaseHelper.addPerson(person);
    person.id = databaseHelper.getPersonId(person);

    //Add mother and father to database

    father.firstName = "FATHER";
    father.lastName = QString::number(person.id);
    father.sex = 'M';
    father.birthdate.setDate(0001, 1, 1);
    databaseHelper.addPerson(father);

    //Add mother to database
    mother.firstName = "MOTHER";
    motherId = person.id;
    mother.lastName = QString::number(person.id);
    mother.sex = 'F';
    mother.birthdate.setDate(0001, 1, 1);
    databaseHelper.addPerson(mother);

    fatherId = databaseHelper.getFatherId("FATHER", QString::number( person.id));
    motherId = databaseHelper.getMotherId("MOTHER", QString::number( person.id));
    databaseHelper.addParents(person.id, fatherId, motherId);
    person.fatherId = fatherId;
    person.motherId = motherId;
    pedigree.createPedigree(person, databaseHelper);
}
