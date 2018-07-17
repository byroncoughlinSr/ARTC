#include "person.h"

Person::Person()
{

}

QString getFirstName(Person::Individual individual)
{
    return individual.firstName;
}

void Person::setFirstName(QString fname, Person::Individual individual)
{
    individual.firstName = fname;
}

QString getLastName(Person::Individual individual)
{
    return individual.lastName;
}

void Person::setLastName(QString lname, Person::Individual individual)
{
    individual.lastName = lname;
}

QString getName(Person::Individual individual)
{
    return individual.name;
}

void Person::setName(QString name, Person::Individual individual)
{
    individual.name = name;
}

QString getAlias(Person::Individual individual)
{
    return individual.alias;
}

void Person::setAlias(QString alias, Person::Individual individual)
{
    individual.alias = alias;
}

void Person::setSex(char sex, Person::Individual individual)
{
    individual.sex = sex;
}

char getSex(Person::Individual individual)
{
    return individual.sex;
}

void Person::setMother(int id, Person::Individual individual)
{
   individual.motherId = id;
}

int Person::getMotherId(Person::Individual individual)
{
    return individual.motherId;
}

