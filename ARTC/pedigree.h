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
    DatabaseHelper *databaseHelper;

   void createPedigree(int id);
   void nextGeneration(int id);
   void createSide(QChar s, int id);
   void addParents();


private:
   Person::Individual new_person;
   Person::PedigreeConstant pedigreeConstant;
};

#endif // PEDIGREE_H
