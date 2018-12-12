#ifndef LINKEDLIST_STRUCT_ITEM_H
#define LINKEDLIST_STRUCT_ITEM_H
#include <QRectF>
#include <QWidget>
#include <QGraphicsTextItem>

struct listNode;
typedef listNode* ptrType;

struct NewLeaf
{
    QRectF rect;
    QGraphicsTextItem *qgraphicsTextItem;
    int id;
    QString name;
};

class linkedlist_struct_item
{
public:
    linkedlist_struct_item();
    linkedlist_struct_item(const linkedlist_struct_item& L);
    ~linkedlist_struct_item();

    // list operations:
    bool listIsEmpty() const;
    int listLength() const;
    void listInsert(int newPosition, NewLeaf newItem, bool& success);
    void listDelete(int position, bool& success);
    void listRetrieve(int position, NewLeaf &dataItem, bool& success) const;

private:
    int size;
    ptrType head;
    ptrType ptrTo(int position) const;
};

#endif // LINKEDLIST_STRUCT_ITEM_H
