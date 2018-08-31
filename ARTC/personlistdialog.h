#ifndef PERSONLISTDIALOG_H
#define PERSONLISTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class PersonListDialog;
}

class PersonListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PersonListDialog(QWidget *parent = 0);
    ~PersonListDialog();

private:
    Ui::PersonListDialog *ui;
     QSqlQueryModel *model=new QSqlQueryModel;
};

#endif // PERSONLISTDIALOG_H
