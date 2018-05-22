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
    string name;
    string firstName;
    string middleName;
    string lastName;
    QDate birthdate;
    char sex;
    int mtherId;
    int fatherId;
    };

   public:
    string getFirstName();
    void setFirstName(string);
    string getLastName();
    void setLastName(string);
    string getName();
    void setName();
    string getAlias();
    void setAlias(string);
};

#endif // PERSON_H
