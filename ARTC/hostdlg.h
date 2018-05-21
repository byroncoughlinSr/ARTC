#ifndef HOSTDLG_H
#define HOSTDLG_H

#include <QDialog>
#include "person.h"
#include <string>
using namespace std;

namespace Ui {
class Hostdlg;
}

class Hostdlg : public QDialog
{
    Q_OBJECT

private:
    Person::Individual host;


public:
    explicit Hostdlg(QWidget *parent = 0);
    ~Hostdlg();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Hostdlg *ui;
};

#endif // HOSTDLG_H
