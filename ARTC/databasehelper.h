#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"


class DatabaseHelper
{
public:
    DatabaseHelper(QString, QString);
    bool createConnection();
    bool addPerson(struct Person::Individual p);
    int getPersonId(struct Person::Individual);
    bool setHost(int id);
    bool addMother(int id);
    bool addFather(int id);
    int getFatherId(QString fn, QString ln);
    int getMotherId(QString fn, QString ln);
    bool addParents(int person, int fatherId, int motherId);

 private:
    QString userName;
    QString passWord;
};

#endif // DATABASEHELPER_H
