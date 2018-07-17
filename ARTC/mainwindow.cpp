#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hostdlg.h"
#include "login.h"
#include "databasehelper.h"

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
    int hostId;
    int fatherId;
    int motherId;
    Hostdlg hostdlg;

    hostdlg.setModal(true);
    hostdlg.exec();

    databaseHelper.addPerson(hostdlg.getHost());
    hostId = databaseHelper.getPersonId(hostdlg.getHost());
    databaseHelper.addFather(hostId);
    databaseHelper.addMother(hostId);
    fatherId = databaseHelper.getFatherId("FATHER", QString::number(hostId));
    motherId = databaseHelper.getMotherId("MOTHER", QString::number(hostId));
    databaseHelper.addParents(hostId, fatherId, motherId);
    databaseHelper.setHost(hostId);
}
