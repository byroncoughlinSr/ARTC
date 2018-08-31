#ifndef PEDIGREE_H
#define PEDIGREE_H

#include "person.h"
#include "databasehelper.h"
#include <string>

using namespace std;

class Pedigree
{
public:
    int generation;
    int max_number;
    int person;

   void createPedigree(int id, DatabaseHelper databaseHelper);
   void nextGeneration(int id, DatabaseHelper databasehelper);
   void createSide(QChar s, int id, DatabaseHelper databaseHelper);
   void addParents(DatabaseHelper databaseHelper);


private:
   Person::Individual new_person;
   Person::PedigreeConstant pedigreeConstant;
};

#endif // PEDIGREE_H
