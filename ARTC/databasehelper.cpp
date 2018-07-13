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

bool DatabaseHelper::addPerson(struct Person::Individual p)
{
    QString fn = p.firstName;
    QString mn = p.middleName;
    QString ln =p.lastName;
    QString bd = p.birthdate.toString("yyyy.MM.dd");
    QChar sex = p.sex;

    QSqlQuery query;
    query.exec("INSERT INTO tblPerson (firstName, middleName, lastName, birthdate, sex)"
               "VALUES('" + fn + "', '" + mn + "', '" + ln + "', '" + bd + "', '" + sex +"')");

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
