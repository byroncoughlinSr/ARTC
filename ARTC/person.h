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
    char sex;
    QDate birthdate;
    int motherId;
    int fatherId;
    };

public:
Individual individual;
QString getFirstName();
void setFirstName(QString);
QString getLastName();
void setLastName(QString);
QString getName();
void setName(QString);
QString getAlias();
void setAlias(QString);
void setSex(char s);
char getSex();
void setMother(int id);
int getMotherId();
};

#endif // PERSON_H
