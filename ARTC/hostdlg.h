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
    Ui::Hostdlg *ui;
    Person::Individual host;


public:
    explicit Hostdlg(QWidget *parent = 0);
    ~Hostdlg();
    Person::Individual getHost();



private slots:
    void on_buttonBox_accepted();    
};

#endif // HOSTDLG_H
