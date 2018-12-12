#include "linkedlist.h"
#include <stddef.h>
#include <assert.h>

struct listNode
{
    listItemType item;
    ptrType next;
};


linkedList::linkedList(): size(0), head(NULL)
{

}

/*!
 * \brief linkedList::linkedList
 * \param L
 */
linkedList::linkedList(const linkedList &L): size(L.size)
{
    if(L.head == NULL)
        head = NULL;
    else
    {
        head = new listNode;
        assert(head != NULL);
        head->item = L.head->item;
        ptrType newPtr = head;
        for (ptrType origPtr = L.head->next; origPtr != NULL; origPtr = origPtr->next)
        {
            newPtr->next = new listNode;
            assert(newPtr->next != NULL);
            newPtr = newPtr->next;
            newPtr->item = origPtr->item;
        }

        newPtr->next = NULL;
    }
}

/*!
 * \brief linkedList::~linkedList
 */
linkedList::~linkedList()
{
    bool success;

    while (!listIsEmpty())
        listDelete(1, success);
}

/*!
 * \brief linkedList::listIsEmpty
 * \return
 */
bool linkedList::listIsEmpty() const
{
    return bool(size == 0);
}

/*!
 * \brief linkedList::listLength
 * \return
 */
int linkedList::listLength() const
{
    return size;
}


/*!
 * \brief linkedList::ptrTo
 * \param position
 * \return
 */
ptrType linkedList::ptrTo(int position) const
{
    if ( (position < 1) || (position > listLength()) )
        return NULL;
    else
    {
        ptrType cur = head;
        for (int skip = 1; skip < position; ++ skip)
                cur = cur->next;
        return cur;
    }
}

/*!
 * \brief linkedList::listRetrieve
 * \param position
 * \param dataItem
 * \param success
 */
void linkedList::listRetrieve(int position, listItemType &dataItem, bool &success) const
{
    success = bool( (position >= 1) &&
                    (position <= listLength()));
    if (success)
    {
        ptrType cur = ptrTo(position);
        dataItem = cur->item;
    }
}

/*!
 * \brief linkedList::listInsert
 * \param newPosition
 * \param newItem
 * \param success
 */
void linkedList::listInsert(int newPosition, listItemType newItem, bool &success)
{
    int newLength = listLength() + 1;

    success = bool( (newPosition >= 1) &&
                    (newPosition <= newLength));

    if (success)
    {
        ptrType newPtr = new listNode;
        success = bool(newPtr != NULL);
        if (success)
        {
            size = newLength;
            newPtr->item = newItem;

            if (newPosition == 1)
            {
                newPtr->next = head;
                head = newPtr;
            }
            else
            {
                ptrType prev = ptrTo(newPosition-1);
                newPtr->next = prev->next;
                prev->next = newPtr;
            }
        }
    }
}

/*!
 * \brief linkedList::listDelete
 * \param position
 * \param success
 */
void linkedList::listDelete(int position, bool &success)
{
    ptrType cur;

    success = bool( ( position >= 1) &&
                    (position <= listLength()));

    if(success)
    {
        size--;
        if(position == 1)
        {
            cur = head;
            head = head->next;
        }
        else
        {
            ptrType prev = ptrTo(position-1);
            cur = prev->next;
            prev->next = cur->next;
        }

        cur->next = NULL;
        delete cur;
        cur = NULL;
    }
}





