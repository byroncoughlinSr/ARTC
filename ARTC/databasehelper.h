#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include "person.h"
#include <QSqlDatabase>
#include <QtSql>


class DatabaseHelper
{
public:
    DatabaseHelper(QString, QString);
    bool createConnection();
    QString lastError() const;
    bool addPerson(struct Person::Individual p);

    /**
     * @brief Insert an empty slot and return its id.
     *
     * Replaces the addPerson-then-look-the-name-up-again round trip: the id
     * comes straight back from the insert, so two slots can never be confused
     * for sharing a generated name.
     *
     * @param slotCode the position code, e.g. "MGF201-7"
     * @param sex 'M' or 'F'
     * @return the new tblPerson.ID, or 0 if the insert failed
     */
    int addSlot(const QString &slotCode, QChar sex);
    int getPersonId(struct Person::Individual);
    Person::Individual *getPerson(int id);
    bool setHost(int id);
    bool addMother(struct Person::Individual child);
    bool addFather(struct Person::Individual child);
    int getFatherId(QString fn, QString ln);
    int getMotherId(QString fn, QString ln);
    bool addParents(int person, int fatherId, int motherId);
    bool addChild(int personId, QChar parent, int parentId);
    int getChildsMotherId(int childId);
    int getChildsFatherId(int childId);

 private:
    QString userName;
    QString passWord;
    QString errorText;
    QSqlDatabase db;
    QSqlQuery query;
    int result;
};

#endif // DATABASEHELPER_H
