#include <QSqlDriver>
#include <QSqlDatabase>
#include <QtDebug>
#include <QMessageBox>
#include <QtSql>
#include <QDate>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include "person.h"
#include "databasehelper.h"


namespace {

/**
 * @brief Locate config.ini.
 *
 * Checked in order: $ARTC_CONFIG, then beside the executable, then one
 * directory up — the shadow-build layout, where the binary sits in
 * build-ARTC-Desktop-Debug/ and the file in the project root.
 *
 * @return path to the file, or an empty string if none was found
 */
QString locateConfigFile()
{
    const QString fromEnv = qEnvironmentVariable("ARTC_CONFIG");
    if (!fromEnv.isEmpty()) {
        return fromEnv;
    }

    const QDir appDir(QCoreApplication::applicationDirPath());
    const QStringList candidates = {
        appDir.filePath("config.ini"),
        appDir.filePath("../config.ini"),
    };

    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }

    return QString();
}

} // namespace


DatabaseHelper::DatabaseHelper(QString usernme, QString passWrd)
{
    DatabaseHelper::userName = usernme;
    DatabaseHelper::passWord = passWrd;
}

/**
 * @brief DatabaseHelper::lastError
 * @return why the most recent createConnection() call failed
 */
QString DatabaseHelper::lastError() const
{
    return errorText;
}

/**
 * @brief DatabaseHelper::createConnection
 *
 * The connection target comes from the [connector_artc] group of config.ini;
 * the credentials come from the sign-in screen. Failures are reported through
 * lastError() rather than a dialog, so the caller chooses how to show them.
 *
 * @return true if the database was opened
 */
bool DatabaseHelper::createConnection()
{
    errorText.clear();

    const QString configPath = locateConfigFile();
    if (configPath.isEmpty()) {
        errorText = QObject::tr("No config.ini found. Copy config.ini.example, or set "
                                "ARTC_CONFIG to the file's path.");
        return false;
    }

    QSettings config(configPath, QSettings::IniFormat);
    config.beginGroup("connector_artc");
    const QString host = config.value("host", "127.0.0.1").toString();
    const int port = config.value("port", 3306).toInt();
    const QString database = config.value("database", "dbArtc").toString();
    // Empty credentials mean "use the application's own account". Sign-in is
    // checked against tblAccount now, so the connection no longer carries the
    // end user's credentials and is opened once with the account in config.ini.
    const QString user = userName.isEmpty()
                             ? config.value("user").toString() : userName;
    const QString secret = userName.isEmpty()
                               ? config.value("password").toString() : passWord;
    config.endGroup();

    if (user.isEmpty()) {
        errorText = QObject::tr("No database account in %1. Run "
                                "database/scripts/initial_setup.py.").arg(configPath);
        return false;
    }

    const QString DRIVER("QMYSQL");
    db = QSqlDatabase::addDatabase(DRIVER);

    // isDriverAvailable() only reports that Qt knows the driver's name: it
    // still says true when the plugin is present but cannot be dlopen'd.
    // addDatabase hands back an invalid database in that case, which is the
    // only reliable signal, and the usual cause is a missing client library.
    if (!db.isValid()) {
        errorText = QObject::tr(
            "The %1 driver could not be loaded.\n\n"
            "Qt's plugin needs the MySQL client library it was built against "
            "(libmysqlclient.so.21). Run with QT_DEBUG_PLUGINS=1 to see which "
            "library is missing.").arg(DRIVER);
        return false;
    }
    db.setHostName(host);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(secret);
    db.setDatabaseName(database);

    if (!db.open()) {
        errorText = QObject::tr("Could not open %1 at %2:%3 as '%4'.\n"
                                "Settings read from %5.\n\n%6")
                        .arg(database, host, QString::number(port), user,
                             configPath, db.lastError().text());
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
 * @brief DatabaseHelper::addSlot
 * @param slotCode
 * @param sex
 * @return
 */
int DatabaseHelper::addSlot(const QString &slotCode, QChar sex)
{
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "INSERT INTO tblPerson (slotCode, gender) VALUES (:slot, :sex)"));
    query.bindValue(QStringLiteral(":slot"), slotCode);
    query.bindValue(QStringLiteral(":sex"), QString(sex));

    if (!query.exec()) {
        QMessageBox::critical(nullptr, QObject::tr("Query Error"),
                              query.lastError().text());
        return 0;
    }

    return query.lastInsertId().toInt();
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
