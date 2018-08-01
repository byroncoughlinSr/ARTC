#include "pedigree.h"
#include <QSqlDatabase>
#include "databasehelper.h"
#include "person.h"
#include <cmath>

void Pedigree::createPedigree(int id, DatabaseHelper databaseHelper)
{
    QString mother;
    QString father;
    QString person;

    mother = "MOTHER-" + QString::number(id,10);
    new_person.firstName = mother;
    databaseHelper.addPerson(new_person);
    father = "FATHER-" + QString::number(id,10);
    new_person.firstName = father;
    databaseHelper.addPerson(new_person);

    generation = 3;
    max_number = 4;

    while (generation < 7)
    {
        nextGeneration(id, databaseHelper);
    }
}

void Pedigree::nextGeneration(int id, DatabaseHelper databasehelper)
{
    QChar side = 'M';
    person=1;

   while (person <= max_number)
   {
        //Create father's side
        createSide(side, id, databasehelper);

        //Create mother's side
        side = 'F';
        createSide(side, id, databasehelper);
   }
   generation++;
   //For each generation number of individuals doubles
    max_number = max_number*2;
}


//Create one side of the pedigree chart up to five generations
void Pedigree::createSide(QChar s, int id, DatabaseHelper databaseHelper)
{
    Person next_person;
    QString name;
    QString par;
    QString gen;
    int sequence;
    int num;
    int parent;
    QString grandparent;
    num = max_number/2;
    parent = 1;

      for (sequence = 1; sequence <= num; sequence++)
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
          if (parent < 10)
          {
              par = "0" + QString::number(parent, 10);
          } else {
              par = QString::number(parent, 10);
          }

          if (generation < 10)
          {
              gen = "0" + QString::number(generation, 10);
          } else {
              gen = QString::number(generation, 10);
          }
          name =  s + grandparent + gen + par + "-" + QString::number(id, 10);
          if(grandparent == "GM")
          {
             parent++;
          }
          next_person.setFirstName(name);
          if (grandparent == "GF")
          {
              next_person.setSex('M');
          } else
          {
             next_person.setSex('F');
          }
          databaseHelper.addPerson(next_person.individual);
          person++;
      }     
}
