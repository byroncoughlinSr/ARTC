#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hostdlg.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_New_Host_triggered()
{
    Hostdlg hostdlg;
    hostdlg.setModal(true);
    hostdlg.exec();
}
