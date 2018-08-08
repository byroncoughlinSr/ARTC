#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"
#include <QSqlDatabase>

class DatabaseHelper
{
public:
    DatabaseHelper(QString, QString);
    bool createConnection();
    int addPerson(struct Person::Individual p);
    bool addPedigreeConstant(struct Person::PedigreeConstant);
    int getPersonId(struct Person::Individual);
    bool setHost(int id);
    bool addMother(int id);
    bool addFather(int id);
    int getFatherId(QString fn, QString ln);
    int getMotherId(QString fn, QString ln);
    bool addParents(int person, int fatherId, int motherId);
    int getMaxGeneration();
    QSqlQuery getGeneration(int generatoin, QChar person, QString grandparent);

 private:
    QString userName;
    QString passWord;
};

#endif // DATABASEHELPER_H
