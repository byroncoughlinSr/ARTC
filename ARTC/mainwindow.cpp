#include "mainwindow.h"
#include <QSqlQueryModel>
#include "ui_mainwindow.h"
#include "hostdlg.h"
#include "login.h"
#include "databasehelper.h"
#include "personlistdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Login login;

    //Run login dialog
    login.setModal(true);
    login.exec();

    DatabaseHelper *databaseHelper = DatabaseHelper::getInstance();
    databaseHelper->createConnection();

    ui->setupUi(this);
    model=databaseHelper->getListView();
    ui->individualListView->setModel(model);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    scene->addRect(10,10,100,100);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_Host_triggered()
{
    int hostId;
    Hostdlg hostdlg;

    hostdlg.setModal(true);
    hostdlg.exec();

    databaseHelper.addPerson(hostdlg.getHost());
    hostId = databaseHelper.getPersonId(hostdlg.getHost());
    databaseHelper.setHost(hostId);
    pedigree.createPedigree(hostId, databaseHelper);
}

void MainWindow::on_individualListView_doubleClicked(const QModelIndex &index)
{
    QString val=ui->individualListView->model()->data(index).toString();


}

void MainWindow::on_actionPedigree_triggered()
{
    PersonListDialog personList;

    personList.setModal(true);
    personList.exec();
}
