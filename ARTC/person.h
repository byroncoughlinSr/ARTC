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
    int id = 0;
    QString name = NULL;
    QString firstName = NULL;
    QString middleName = NULL;
    QString lastName = NULL;
    QString alias = NULL;
    QChar sex;
    QDate birthdate;
    QDate deathdate;
    int motherId = 0;
    int fatherId = 0;
    Individual* next = NULL;
    };

    public:
};

#endif // PERSON_H
