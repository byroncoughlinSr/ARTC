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
    string firstName;
    string middleName;
    string lastName;
    QDate birthdate;
    char sex;
    int mtherId;
    int fatherId;
    };
};

#endif // PERSON_H
