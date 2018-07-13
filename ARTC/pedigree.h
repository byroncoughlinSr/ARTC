#ifndef PEDIGREE_H
#define PEDIGREE_H

#include "person.h"
#include <string>
using namespace std;

class Pedigree
{
public:
    Pedigree();
   void createPedigree(Person::Individual individual );
   void createSide(QChar s, int id);
};

#endif // PEDIGREE_H
