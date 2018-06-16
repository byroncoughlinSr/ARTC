#include "pedigree.h"

Pedigree::Pedigree()
{

}

void Pedigree::createPedigree(Person::Individual individual)
{
    int id;
    int generation = 1;
    char side = 'M';
    QString mother;
    QString father;

    id = individual.id;
    mother = "MOTHER-" + QString::number(id,10);
    father = "FATHER-" + QString::number(id,10);
}
