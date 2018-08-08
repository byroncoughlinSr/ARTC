#include "databasehelper.h"
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QtDebug>
#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include "person.h"


DatabaseHelper::DatabaseHelper(QString usernme, QString passWrd)
{
    DatabaseHelper::userName = usernme;
    DatabaseHelper::passWord = passWrd;   
}


bool DatabaseHelper::createConnection()
{
    const QString DRIVER("QMYSQL");
    QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
    db.setDatabaseName("artc");
    db.setHostName("192.168.56.103");
    db.setUserName(userName);
    db.setPassword(passWord);
    db.setDatabaseName("dbArtc");
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
        return false;
    }
    return true;
}

int DatabaseHelper::addPerson(struct Person::Individual p)
{
    QString fn = p.firstName;
    QString mn = p.middleName;
    QString ln =p.lastName;
    QString bd = p.birthdate.toString("yyyy.MM.dd");
    QChar sex = p.sex;

    QSqlQuery query;
    query.exec("INSERT INTO tblPerson (firstName, middleName, lastName, birthdate, sex)"
               "VALUES('" + fn + "', '" + mn + "', '" + ln + "', '" + bd + "', '" + sex +"')");

    return query.lastInsertId().toInt();
}

bool DatabaseHelper::addPedigreeConstant(struct Person::PedigreeConstant ped)
{
    QSqlQuery qry;
    qry.prepare("INSERT INTO tblPedigreeConstant (pedPersonId, pedConstant, pedGrandparentChar, pedParentChar, pedGeneration, pedSequence, pedRootId) VALUES (:id, :name, :grandparent, :parent, :generation, :sequence, :root);");
    qry.bindValue(":id", ped.personId);
    qry.bindValue(":name", ped.pedigreeConstanName);
    qry.bindValue(":grandparent", ped.pedigreeConstantGrandparent);
    qry.bindValue(":parent", ped.pedigreeConstantParent);
    qry.bindValue(":generation", ped.pedigreeGeneration);
    qry.bindValue(":sequence", ped.pedigreeSequence);
    qry.bindValue(":root", ped.pedigreeRootId);

    qry.exec();

            return true;
}

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
bool DatabaseHelper::addMother(int id)
{
    QString fn = "MOTHER";
    QString ln = QString::number(id);

    QSqlQuery query;
    query.exec("INSERT INTO tblPerson (firstName, lastName)"
               "VALUES('" + fn + "', '" + ln + "')");
    return true;
}

/**
 * @brief DatabaseHelper::addFather
 * @param id
 * @return
 */
bool DatabaseHelper::addFather(int id)
{
    QString fn = "FATHER";
    QString ln = QString::number(id);

    QSqlQuery query;
    query.exec("INSERT INTO tblPerson (firstName, lastName)"
               "VALUES('" + fn + "', '" + ln + "')");
    return true;
}

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

 bool DatabaseHelper::addParents(int personId, int fatherId, int motherId)
 {
     QSqlQuery qry;
     qry.prepare("UPDATE tblPerson SET fatherId = :fatherId, motherId = :motherId WHERE ID = :personId;");
     qry.bindValue(":fatherId", fatherId);
     qry.bindValue(":motherId", motherId);
     qry.bindValue(":personId", personId);
     qry.exec();
     return true;
 }

 int DatabaseHelper::getMaxGeneration()
 {
     int max;
     QSqlQuery query;
     query.exec("SELECT MAX(pedGeneration) FROM tblPedigreeConstant");

     while(query.next())
     {
         max = query.value(0).toInt();
     }

     return max;
 }

 QSqlQuery DatabaseHelper::getGeneration(int generation, QChar person, QString grandparent)
 {
     QSqlQuery query;
     query.prepare("SELECT * FROM tblPedigreeConstant WHERE pedGeneration = :generation AND pedParentChar = :parent AND pedGrandparentChar = :grandparent" );
     query.bindValue(":generation", generation );
     query.bindValue(":parent", person );
     query.bindValue(":grandparent", grandparent );
     query.exec();

     return query;

 }

