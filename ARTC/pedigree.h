#ifndef PEDIGREE_H
#define PEDIGREE_H

#include "person.h"
#include <string>
#include "databasehelper.h"
using namespace std;

class Pedigree
{
public:
    struct membersLL {
        int id;
        membersLL *next;
   };

public:
   Pedigree();
   void createPedigree(Person::Individual individual, DatabaseHelper dh );
   void createSide(QChar s, int id, DatabaseHelper dbHelper);
   Person::Individual *createNewParents(int sequence);

   membersLL *createRootsParentsLL(DatabaseHelper dh, int id);

private:
   membersLL *head = NULL;
   membersLL *temp = NULL;
   membersLL *tail = NULL;
   membersLL *temp1 = NULL;
   membersLL *tail1 = NULL;
   membersLL *head1 = NULL;
   QDate defautltDate;
   const int MAX_GENERATION = 7;
   int generation;
   int rootId;
   void add_node(int m);
   void copyLinkList();
};

#endif // PEDIGREE_H
