#include <iostream>
#include "pedigreetreepaint.h"
#include "person.h"
#include "databasehelper.h"
#include <QDebug>
#include <QPainter>
#include <QGraphicsTextItem>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QRectF>
#include "leaf.h"

pedigreeTreePaint::pedigreeTreePaint(QWidget *parent)
    : QGraphicsView(parent)
{
    this->setAlignment(Qt::AlignCenter);                        // Make the contents of binding to the center
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Stretch the widget content

        this->setMinimumHeight(100);
        this->setMinimumWidth(100);

}

void pedigreeTreePaint::createTree(int root, QGraphicsScene *scene)
{
    QGraphicsTextItem *textItem;
    int i =1;
    current_ll = new linkedList();
    nextGen_ll = new linkedList();
    leaf_ll = new linkedlist_struct_item();

    gen = 1;
    pos = 1;
    id = root;
    dbHelper = DatabaseHelper::getInstance();


    nextPerson = dbHelper->getPersonFromId(id);
    current_ll->listInsert(pos, id, success);

    newLeaf.id = id;
    newLeaf.name = nextPerson.name;
    newLeaf.rect.setRect(x, y, width, height);
    leaf_ll->listInsert(ll_pos, newLeaf, success);
    x=x+140;
    y=y+30;

     while (gen < 2)
    {
       nextGeneration(scene);

    }

     for (i=1; i <= leaf_ll->listLength(); i++)
     {
         leaf_ll->listRetrieve(i, newLeaf, success);
         leaf = new Leaf;
         leaf->setPos(x, y);
         scene->addItem(leaf);
         x = -120.0;
         y= y + 200;
     }

}

void pedigreeTreePaint::nextGeneration(QGraphicsScene *scene)
{
     pos = 1;
     int parent = 1;
     int fatherId;
     int motherId;


     //traverse the listx = x + 230.0;
    while(pos <= max_number)
    {
        current_ll->listRetrieve(pos, id, success);
        nextPerson = dbHelper->getPersonFromId(id);

        fatherId = nextPerson.fatherId;
        createLeaf(fatherId);
        ll_pos++;
        leaf_ll->listInsert(ll_pos, newLeaf, success);

        motherId = nextPerson.motherId;
        y=y-60;https://www.google.com/
        createLeaf(motherId);
        ll_pos++;
        leaf_ll->listInsert(ll_pos, newLeaf, success);

        y=y-90;

        nextGen_ll->listInsert(parent, fatherId, success);
        parent++;
        nextGen_ll->listInsert(parent, motherId, success);
        parent++;
        pos++;
    }
    delete current_ll;
    current_ll = new linkedList(*nextGen_ll);
    delete nextGen_ll;
    nextGen_ll = new linkedList();

    gen++;
    //For each generatUntitled 1ion number of individuals doubles
     max_number = max_number*2;
     x=x+140.0;
     y = max_number * 5.0;
}

void pedigreeTreePaint::createLeaf(int id)
{
    Person::Individual person = dbHelper->getPersonFromId(id);
    newLeaf.name = person.name;
    newLeaf.id = id;
    newLeaf.rect.setRect(x, y, width, height);
}

pedigreeTreePaint::~pedigreeTreePaint()
{

}
