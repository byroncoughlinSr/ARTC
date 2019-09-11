#include <QSqlDriver>
#include <QSqlDatabase>
#include <QtDebug>
#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include "person.h"
#include "databasehelper.h"


DatabaseHelper::DatabaseHelper(QString usernme, QString passWrd)
{
    DatabaseHelper::userName = usernme;
    DatabaseHelper::passWord = passWrd;   
}

/**
 * @brief DatabaseHelper::createConnection
 * @return
 */
bool DatabaseHelper::createConnection()
{
    const QString DRIVER("QMYSQL");
    db = QSqlDatabase::addDatabase(DRIVER);
    db.setHostName("172.28.1.5");
    db.setUserName(userName);
    db.setPassword(passWord);
    db.setDatabaseName("dbArtc");
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        return false;
    }
    return true;
}

/**
 * @brief DatabaseHelper::addPerson
 * @param p
 * @return
 */
bool DatabaseHelper::addPerson(struct Person::Individual p)
{
    QString fn = p.firstName;
    QString mn = p.middleName;
    QString ln =p.lastName;
    QString bd = p.birthdate.toString("yyyy-MM-dd");
    QChar gender = p.sex;

    QSqlQuery query;

    if (!query.exec("INSERT INTO tblPerson (firstName, middleName, lastName, gender, birthdate)"
               "VALUES('" + fn + "', '" + mn + "', '" + ln + "', '" + gender + "', '" + bd + "')")) {
        QMessageBox msgBox;
        msgBox.critical(0, QObject::tr("Query Error"), query.lastError().text());
        return false;
    }    

    return true;
}




/**
 * @brief DatabaseHelper::getPersonId
 * @param p
 * @return
 */
int DatabaseHelper::getPersonId(struct Person::Individual p)
{
    QString fn = p.firstName;
    QString mn = p.middleName;
    QString ln = p.lastName;
    int id;

    QSqlQuery query;
    query.exec("SELECT ID FROM tblPerson WHERE firstname = '" + fn + "' AND middleName = '" + mn + "' AND lastName = '" + ln + "'");

    while (query.next())
    {
        id = query.value(0).toInt();
    }

    return id;
}

/**
 * @brief DatabaseHelper::setHost
 * @param hostId
 * @return
 */
bool DatabaseHelper::setHost(int hostId)
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO tblHost (personId) VALUES (:id);");
    qry.bindValue(":id", hostId);
    qry.exec();
    return true;
}

/**
 * @brief DatabaseHelper::addMother
 * @param id
 * @return
 */
bool DatabaseHelper::addMother(struct Person::Individual child)
{
    QString fn = "MOTHER";
    int childId = child.id;
    QString ln = QString::number(childId);


    QSqlQuery query;
   if (!query.exec("INSERT INTO tblPerson (firstName, lastName)"
               "VALUES('" + fn + "', '" + ln + "')")) {
        QMessageBox msgBox;
        msgBox.critical(0, QObject::tr("Query Error"), query.lastError().text());
        return false;
    }
    return true;
}
/**
 * @brief DatabaseHelper::addFather
 * @param id
 * @return
 */
bool DatabaseHelper::addFather(struct Person::Individual child)
{
    QString fn = "FATHER";
    int childId = child.id;
    QString ln = QString::number(childId);

    QSqlQuery query;
    if (!query.exec("INSERT INTO tblPerson (firstName, lastName)"
                "VALUES('" + fn + "', '" + ln + "')")) {
         QMessageBox msgBox;
         msgBox.critical(0, QObject::tr("Query Error"), query.lastError().text());
         return false;
     }
     return true;
 }

/**
 * @brief DatabaseHelper::getFatherId
 * @param fn
 * @param ln
 * @return
 */
int DatabaseHelper::getFatherId(QString fn, QString ln)
{
    int id;

    QSqlQuery query;
    query.exec("SELECT ID FROM tblPerson WHERE firstname = '" + fn + "' AND lastName = '" + ln + "'");

    while (query.next())
    {
        id = query.value(0).toInt();
    }

    return id;
}

/**
 * @brief DatabaseHelper::getMotherId
 * @param fn
 * @param ln
 * @return
 */
int DatabaseHelper::getMotherId(QString fn, QString ln)
{
    int id;

    QSqlQuery query;
    query.exec("SELECT ID FROM tblPerson WHERE firstname = '" + fn + "' AND lastName = '" + ln + "'");

    while (query.next())
    {
        id = query.value(0).toInt();
    }

    return id;
}

/**
 * @brief DatabaseHelper::addParents
 * @param personId
 * @param fatherId
 * @param motherId
 * @return
 */
bool DatabaseHelper::addParents(int personId, int fatherId, int motherId)
 {
     QSqlQuery qry;
     qry.prepare("UPDATE tblPerson SET fatherId = :fatherId, motherId = :motherId WHERE ID = :personId;");
     qry.bindValue(":fatherId", fatherId);
     qry.bindValue(":motherId", motherId);
     qry.bindValue(":personId", personId);
     qry.exec();

     addChild(personId, 'F', fatherId);
     addChild(personId, 'M', motherId);
     return true;
 }

/**
 * @brief DatabaseHelper::addChild
 * @param personId
 * @param parent
 * @param parentId
 * @return
 */
bool DatabaseHelper::addChild(int personId, QChar parent, int parentId) {
     QSqlQuery qry;

     if (parent == 'F')
     {
        qry.prepare("INSERT INTO tblChild (parentId, childId) VALUES(:fatherId, :personId)");
        qry.bindValue(":fatherId", parentId);
        qry.bindValue(":personId", personId);
        qry.exec();
     }
    else
    {
        qry.prepare("INSERT INTO tblChild (parentId, childId) VALUES(:motherId, :personId)");
        qry.bindValue(":motherId", parentId);
        qry.bindValue(":personId", personId);
        qry.exec();
    }
     return true;
 }

 /**
  * @brief DatabaseHelper::getChildsFatherId
  * @param childId
  * @return
  */
 int DatabaseHelper::getChildsFatherId(int childId) {
     QSqlQuery query;
     int fathersId;

     query.prepare("SELECT fatherId FROM tblPerson WHERE ID = :childId");
     query.bindValue(":childId", childId);
     query.exec();

     while (query.next()) {
         fathersId = query.value(0).toInt();
     }
     return fathersId;
 }

 /**
  * @brief DatabaseHelper::getChildsMotherId
  * @param childId
  * @return
  */
 int DatabaseHelper::getChildsMotherId(int childId) {
     QSqlQuery query;
     int mothersId;

     query.prepare("SELECT motherId FROM tblPerson WHERE ID = :childId");
     query.bindValue(":childId", childId);
     query.exec();

     while (query.next()) {
         mothersId = query.value(0).toInt();
     }
     return mothersId;
 }

 /**
  * @brief getPerson
  * @param id
  * @return
  */
 Person::Individual *DatabaseHelper::getPerson(int id)
 {
     Person::Individual *person;
     QSqlQuery query;
     QString date;

     person = new Person::Individual;

     query.prepare("SELECT ID, lastName, firstName, middleName, fatherId, motherId FROM tblPerson WHERE ID = :id");
     query.bindValue(":id", id);
     query.exec();

     while (query.next()) {
         person->id = query.value(0).toInt();
         person->lastName = query.value(1).toString();
         person->firstName = query.value(2).toString();
         person->middleName = query.value(3).toString();

        // person->sex = query.value(4).toChar();
        // person->birthdate = query.value(5).toDate();
         person->fatherId = query.value(4).toInt();
         person->motherId = query.value(5).toInt();
     }

     return person;
 }
