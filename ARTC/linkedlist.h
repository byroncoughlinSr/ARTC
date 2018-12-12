#ifndef LINKEDLIST_H
#define LINKEDLIST_H
typedef int listItemType;

struct listNode;
typedef listNode* ptrType;

/*!
 * \brief The linkedList class header file
 */
class linkedList
{
public:
    //list constructors and destructors
    linkedList();
    linkedList(const linkedList& L);
    ~linkedList();

    // list operations:
    bool listIsEmpty() const;
    int listLength() const;
    void listInsert(int newPosition, listItemType newItem, bool& success);
    void listDelete(int position, bool& success);
    void listRetrieve(int position, listItemType& dataItem, bool& success) const;

private:
    int size;
    ptrType head;

    ptrType ptrTo(int position) const;
};

#endif // LINKEDLIST_H
