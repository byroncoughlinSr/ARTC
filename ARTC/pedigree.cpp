#include "pedigree.h"
#include "databasehelper.h"
#include "person.h"
#include <cmath>
#include <iostream>

Pedigree::Pedigree()
{

}

/**
 * @brief Pedigree::createPedigree
 * @param rootPerson
 * @param dh
 */
void Pedigree::createPedigree(Person::Individual rootPerson, DatabaseHelper dh)
{
    QChar side;
    QString mother;
    QString father;
    QString person;
    Person::Individual *personFather;
    Person::Individual *personMother;

   rootId = rootPerson.id;

    personFather = dh.getPerson(rootPerson.fatherId);
    personMother = dh.getPerson(rootPerson.motherId);

    //Create fathers side
    createSide('F', personFather->id, dh);

    //Create mothers side
    createSide('M', personMother->id, dh);
}


//Create one side of the pedigree chart up to five generations
/**
 * @brief Pedigree::createSide
 * @param s
 * @param id
 * @param dbHelper
 */
void Pedigree::createSide(QChar s, int id, DatabaseHelper dbHelper)
{
    QString fatherName;
    QString motherName;
    int sequence;
    int generation;
    int fatherId;
    int motherId;
    QString grandparent;
    Person::Individual father;
    Person::Individual mother;   

    //Create the pedigree table for the individual side
    generation = 1;
    sequence = 1;

    //create link list consisting of the passed parent
    temp = new membersLL;
    temp->id = id;
    temp->next = NULL;
    tail = temp;
    head = temp;

    //go to each generation until completed all desired generations
    while (generation < MAX_GENERATION)
    {
        //loop through each generation creating mother and father of child from previous generation
        while (temp != NULL)
        {            
           //Get id from head
           defautltDate.setDate(0001, 1, 1);

           //Create the father
           grandparent = "GF";
           fatherName =  s + grandparent + QString::number(generation, 10) + QString::number(sequence, 10) + "-" + QString::number(rootId, 10);
           father.firstName = fatherName;
           father.lastName = QString::number(rootId, 10);
           father.birthdate = defautltDate;
           father.deathdate = defautltDate;
           father.sex = 'M';
           dbHelper.addPerson(father);

           //Create the mother
           grandparent = "GM";
           motherName =  s + grandparent + QString::number(generation, 10) + QString::number(sequence, 10) + "-" + QString::number(rootId, 10);
           mother.firstName = motherName;
           mother.lastName = QString::number(rootId, 10);
           mother.birthdate = defautltDate;
           mother.deathdate = defautltDate;
           mother.sex = 'F';
           dbHelper.addPerson(mother);

          //add parents to child
          fatherId = dbHelper.getFatherId(father.firstName, father.lastName);
          motherId =  dbHelper.getMotherId(mother.firstName, mother.lastName);
          dbHelper.addParents(temp->id, fatherId, motherId);
          dbHelper.addChild(temp->id, 'F', fatherId);
          dbHelper.addChild(temp->id, 'M', motherId);

          //Create new nodes on linked list
          add_node(fatherId);
          add_node(motherId);

          sequence++;
          temp = temp->next;
         }
        //Copy new link list for next use
        copyLinkList();
        sequence = 1;
        generation ++;
       }
    }

void Pedigree::add_node(int m) {
    temp1 = new membersLL;
    temp1->id = m;
    temp1->next = NULL;

    if (head1 == NULL)
    {
      head1 = temp1;
      tail1 = temp1;
      temp1 = NULL;
    }
    else
    {
        tail1->next = temp1;
        tail1 = temp1;
        temp1 = NULL;
    }
}

//create first linked list consisting of mother and father of root
Pedigree::membersLL *Pedigree::createRootsParentsLL(DatabaseHelper dh, int id) {
    temp = new membersLL;
    //Create current generation with the id of father
    temp->id = dh.getChildsFatherId(id);
    temp->next = NULL;
    head = temp;
    tail = temp;
    //Create current generation of mother
    temp = new membersLL;
    temp->id = dh.getChildsMotherId(id);
    tail->next = temp;
    temp->next = NULL;
return temp;
}

void Pedigree::copyLinkList()
{
    temp = head1;
    head1 = NULL;

}
