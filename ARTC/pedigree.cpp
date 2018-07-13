#include "pedigree.h"
#include "databasehelper.h"
#include "person.h"
#include <cmath>

Pedigree::Pedigree()
{

}

void Pedigree::createPedigree(Person::Individual individual)
{
    int id;
    QChar side = 'M';
    QString mother;
    QString father;
    QString person;

    id = individual.id;
    mother = "MOTHER-" + QString::number(id,10);
    father = "FATHER-" + QString::number(id,10);

    //Create father's side
    createSide(side, id);

    //Create mother's side
    side = 'F';
    createSide(side, id);
}

//Create one side of the pedigree chart up to five generations
void Pedigree::createSide(QChar s, int id)
{   Person::Individual new_individual;
   // DatabaseHelper dbHelper;
    Person new_person;
    QString name;
    int generation;
    int sequence;
    int max_sequence;
    const int MAX_GENERATION = 5;
    QString grandparent;
    max_sequence = 2;


    for (generation = 1; generation < MAX_GENERATION; generation++)
    {
      for (sequence = 1; sequence < max_sequence; sequence++)
      {

         //If the parent of a generation is an even number in the sequence then they should be classified as a grandmother
          if (sequence % 2 == 0)
          {
            grandparent = "GM";
          }
          //If the parent of a generation is an odd number in the sequence then they should be classified as a grandfather
          else
          {
            grandparent = "GF";
          }

          //Put it all together
          name =  s + grandparent + QString::number(generation, 10) + QString::number(sequence, 10) + "-" + QString::number(id, 10);
          new_person.setFirstName(name, new_individual);
         // dbHelper.addPerson(new_individual);
      }
     //For each generation nuber of individuals doubles
      max_sequence = pow(10, 2);
    }
}
