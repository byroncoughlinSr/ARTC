#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"
#include <QSqlDatabase>
#include <QSqlQueryModel>

class DatabaseHelper
{
     static DatabaseHelper *dbHelper;
public:
    DatabaseHelper(QString, QString);
    void setUserId(QString user);
    void setPassword(QString pwd);
    bool createConnection();
    int addPerson(struct Person::Individual p);
    bool addPedigreeConstant(struct Person::PedigreeConstant);
    int getPersonId(struct Person::Individual);
    Person::Individual getPersonFromId(int id);
    bool setHost(int id);
    bool addMother(int id);
    bool addFather(int id);
    int getFatherId(QString fn, QString ln);
    int getMotherId(QString fn, QString ln);
    bool addParents(int person, int fatherId, int motherId);
    int getMaxGeneration();
    bool addHostsParents(int id);
    QSqlQuery getGeneration(int generatoin, QChar person, QString grandparent);
    QSqlQueryModel *getListView();
    static DatabaseHelper *getInstance();

 private:
    DatabaseHelper();
    QString userName;
    QString passWord;
};

#endif // DATABASEHELPER_H
