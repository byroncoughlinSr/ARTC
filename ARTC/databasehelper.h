#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"
#include <QSqlDatabase>
#include <QtSql>


class DatabaseHelper
{
public:
    DatabaseHelper(QString, QString);
    bool createConnection();
    bool addPerson(struct Person::Individual p);
    int getPersonId(struct Person::Individual);
    Person::Individual *getPerson(int id);
    bool setHost(int id);
    bool addMother(struct Person::Individual child);
    bool addFather(struct Person::Individual child);
    int getFatherId(QString fn, QString ln);
    int getMotherId(QString fn, QString ln);
    bool addParents(int person, int fatherId, int motherId);
    bool addChild(int personId, QChar parent, int parentId);
    int getChildsMotherId(int childId);
    int getChildsFatherId(int childId);

 private:
    QString userName;
    QString passWord;
    QSqlDatabase db;
    QSqlQuery query;
    int result;
};

#endif // DATABASEHELPER_H
