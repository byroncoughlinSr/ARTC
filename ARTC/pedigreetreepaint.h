#ifndef PEDIGREETREEPAINT_H
#define PEDIGREETREEPAINT_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QTimer>
#include <QRectF>
#include "person.h"
#include "databasehelper.h"
#include "linkedlist.h"
#include "linkedlist_struct_item.h"
#include "leaf.h"


class pedigreeTreePaint : public QGraphicsView
{
        Q_OBJECT
    public:
        explicit pedigreeTreePaint(QWidget *parent = 0);
        ~pedigreeTreePaint();

public:
    void createTree(int root, QGraphicsScene *scene);
    linkedList *current_ll;
    linkedList *nextGen_ll;
    linkedlist_struct_item *leaf_ll;

    Person::Individual nextPerson;
    int pos;    
    bool success;
    const int MAX_GENERATION=7;
    int person ;
    int max_number=1;
    int gen;
    int id;

    Person::Individual individual;
    DatabaseHelper *dbHelper;
    void nextGeneration(QGraphicsScene *scene);

private:
    NewLeaf newLeaf;
    Leaf  *leaf;
    qreal x = 0.0;
    qreal y = 0.0;
    qreal height = 30.0;
    qreal width = 120.0;
    int ll_pos = 1;
    void createLeaf(int id);
};

#endif // PEDIGREETREEPAINT_H
