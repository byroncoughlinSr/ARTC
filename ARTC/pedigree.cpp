#include "pedigree.h"
#include <QSqlDatabase>
#include "databasehelper.h"
#include "person.h"
#include <cmath>
#include <QtSql>

void Pedigree::createPedigree(int id)
{
    QString mother;
    QString father;
    QString person;
    databaseHelper = DatabaseHelper::getInstance();

    //Add mother to database
    mother = "MOTHER-" + QString::number(id,10);
    new_person.firstName = mother;
    new_person.sex = 'F';
    databaseHelper->addPerson(new_person);

    //Add father tgo database
    father = "FATHER-" + QString::number(id,10);
    new_person.firstName = father;
    new_person.sex = 'M';
    databaseHelper->addPerson(new_person);

    generation = 3;
    max_number = 4;

    while (generation < 7)
    {
        nextGeneration(id);
    }

    //Add mother and father of host
   databaseHelper->addHostsParents(id);

    //Add parents in pedigree tree from generation 3 to generation 6
    addParents();
}

void Pedigree::nextGeneration(int id)
{
    QChar side = 'M';
    person=1;

   while (person <= max_number)
   {
        //Create father's side
        createSide(side, id);

        //Create mother's side
        side = 'F';
        createSide(side, id);
   }
   generation++;
   //For each generation number of individuals doubles
    max_number = max_number*2;
}


//Create one side of the pedigree chart up to five generations
void Pedigree::createSide(QChar s, int id)
{
    Person next_person;
    QString name;
    QString par;
    QString gen;
    QString grandparent;
    int sequence;
    int num;
    int parent;
    int per;
    while (generation < 7)
    {
        nextGeneration(id);
    }
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

          next_person.setFirstName(name);
          next_person.setName(name);
          if (grandparent == "GF")
          {
              next_person.setSex('M');
          } else
          {
             next_person.setSex('F');
          }
          per = databaseHelper->addPerson(next_person.individual);

          //Set pedigree constants for database
          next_person.setPersonId(per);
          next_person.setPedigreeConstantName(name);
          next_person.setPedigreeConstantGrandparent(grandparent);
          next_person.setPedigreeConstantParent(s);
          next_person.setPedigreeGeneration(generation);
          next_person.setPedigreeSequence(parent);
          next_person.setPedigreeRoot(id);

          databaseHelper->addPedigreeConstant(next_person.pedigreeConstant);

          if(grandparent == "GM")
          {
             parent++;
          }
          person++;
      }
}

void Pedigree::addParents()
{
    int max_generation;
    int next_generation;
    int generation = 3;
    int personId;
    int fatherId;
    int motherId;

    next_generation = generation + 1;
    max_generation = databaseHelper->getMaxGeneration();


    while (generation <= max_generation)
    {
        QSqlQuery  query_father_gf = databaseHelper->getGeneration(generation, 'F', "GF");
        QSqlQuery  query_father_gm = databaseHelper->getGeneration(generation, 'F', "GM");
        QSqlQuery  query_father_gf_next = databaseHelper->getGeneration(next_generation,  'F', "GF");
        QSqlQuery  query_father_gm_next = databaseHelper->getGeneration(next_generation,  'F', "GM");
        while (query_father_gf.next() &&
               query_father_gf_next.next() &&
               query_father_gm_next.next() &&
               query_father_gm.next())
        {
            personId = query_father_gf.value(1).toInt();
            fatherId = query_father_gf_next.value(1).toInt();
            motherId = query_father_gm_next.value(1).toInt();
            databaseHelper->addParents(personId, fatherId, motherId);

            query_father_gf_next.next();
            query_father_gm_next.next();
            personId = query_father_gm.value(1).toInt();
            fatherId = query_father_gf_next.value(1).toInt();
            motherId = query_father_gm_next.value(1).toInt();
            databaseHelper->addParents(personId, fatherId, motherId);
        }

        QSqlQuery  query_mother_gf = databaseHelper->getGeneration(generation, 'M', "GF");
        QSqlQuery  query_mother_gm = databaseHelper->getGeneration(generation, 'M', "GM");
        QSqlQuery  query_mother_gf_next = databaseHelper->getGeneration(next_generation,  'M', "GF");
        QSqlQuery  query_mother_gm_next = databaseHelper->getGeneration(next_generation,  'M', "GM");

        while (query_mother_gf.next() &&
               query_mother_gf_next.next() &&
               query_mother_gm_next.next() &&
               query_mother_gm.next())
        {
            personId = query_mother_gf.value(1).toInt();
            fatherId = query_mother_gf_next.value(1).toInt();
            motherId = query_mother_gm_next.value(1).toInt();
            databaseHelper->addParents(personId, fatherId, motherId);

            query_mother_gf_next.next();
            query_mother_gm_next.next();
            personId = query_mother_gm.value(1).toInt();
            fatherId = query_mother_gf_next.value(1).toInt();
            motherId = query_mother_gm_next.value(1).toInt();
            databaseHelper->addParents(personId, fatherId, motherId);
        }

        generation++;
        next_generation++;
    }
}


