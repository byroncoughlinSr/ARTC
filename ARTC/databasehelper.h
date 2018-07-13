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

 private:
    QString userName;
    QString passWord;
};

#endif // DATABASEHELPER_H
