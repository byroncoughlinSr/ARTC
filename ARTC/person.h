#ifndef PERSON_H
#define PERSON_H
#include <string>
#include <QDate>
using namespace std;

class Person
{
public:
    Person();

public:
    struct Individual {
    int id;
    QString name;
    QString firstName;
    QString middleName;
    QString lastName;
    QString alias;
    QDate birthdate;
    char sex;
    int mtherId;
    int fatherId;
    };


   private:
    QString getFirstName(Individual);
    void setFirstName(QString, Individual);
    QString getLastName();
    void setLastName(QString, Individual);
    QString getName();
    void setName(QString, Individual);
    QString getAlias();
    void setAlias(QString, Individual);
};

#endif // PERSON_H
