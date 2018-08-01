#include "person.h"

Person::Person()
{

}

QString Person::getFirstName()
{
    return individual.firstName;
}

void Person::setFirstName(QString fname)
{
    individual.firstName = fname;
}

QString Person::getLastName()
{
    return individual.lastName;
}

void Person::setLastName(QString lname)
{
    individual.lastName = lname;
}

QString Person::getName()
{
    return individual.name;
}

void Person::setName(QString n)
{
    individual.name = n;
}

QString Person::getAlias()
{
    return individual.alias;
}

void Person::setAlias(QString al)
{
    individual.alias = al;
}

void Person::setSex(char s)
{
    individual.sex = s;
}

char Person::getSex()
{
    return individual.sex;
}

void Person::setMother(int id)
{
   individual.motherId = id;
}

int Person::getMotherId()
{
    return individual.motherId;
}



