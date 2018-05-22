#include "person.h"

Person::Person()
{

}

QString getFirstName(Person::Individual individual)
{
    return individual.firstName;
}

void setFirstName(QString fname, Person::Individual individual)
{
    individual.firstName = fname;
}

QString getLastName(Person::Individual individual)
{
    return individual.lastName;
}

void setLastName(QString lname, Person::Individual individual)
{
    individual.lastName = lname;
}

QString getName(Person::Individual individual)
{
    return individual.name;
}

void setName(QString name, Person::Individual individual)
{
    individual.name = name;
}

QString getAlias(Person::Individual individual)
{
    return individual.alias;
}

void setAlias(QString alias, Person::Individual individual)
{
    individual.alias = alias;
}

