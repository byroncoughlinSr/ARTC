#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"


class DatabaseHelper
{
public:
    DatabaseHelper();
    bool createConnection();
    bool addPerson(struct Person::Individual p);
    int getPersonId(struct Person::Individual);
};

#endif // DATABASEHELPER_H
