#include "accountrepository.h"
#include "passwordhasher.h"
#include "registerwidget.h"

#include <QObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {

/** Report @p query's failure through @p error, and return false. */
bool fail(QString *error, const QSqlQuery &query, const QString &context)
{
    if (error) {
        *error = QStringLiteral("%1\n\n%2").arg(context, query.lastError().text());
    }
    return false;
}

void setError(QString *error, const QString &message)
{
    if (error) {
        *error = message;
    }
}

} // namespace

namespace AccountRepository {

bool emailTaken(const QString &email, QString *error)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT 1 FROM tblAccount WHERE email = :email"));
    query.bindValue(QStringLiteral(":email"), email);

    if (!query.exec()) {
        fail(error, query, QObject::tr("Could not check the email address."));
        // Refuse the registration rather than let a failed check look like a
        // free address and hit the unique key instead.
        return true;
    }

    return query.next();
}

bool create(const RegistrationDetails &details, QString *error)
{
    const QString encoded = PasswordHasher::hash(details.password);
    if (encoded.isEmpty()) {
        setError(error, QObject::tr("Could not hash the password. The OpenSSL "
                                    "Argon2id provider is unavailable."));
        return false;
    }

    QSqlQuery query;
    query.prepare(QStringLiteral(
        "INSERT INTO tblAccount (email, passwordHash, firstName, middleName, "
        "lastName, birthdate) "
        "VALUES (:email, :hash, :firstName, :middleName, :lastName, :birthdate)"));
    query.bindValue(QStringLiteral(":email"), details.email);
    query.bindValue(QStringLiteral(":hash"), encoded);
    query.bindValue(QStringLiteral(":firstName"), details.firstName);
    query.bindValue(QStringLiteral(":middleName"),
                    details.middleName.isEmpty() ? QVariant() : details.middleName);
    query.bindValue(QStringLiteral(":lastName"), details.lastName);
    query.bindValue(QStringLiteral(":birthdate"),
                    details.birthdate.isValid() ? QVariant(details.birthdate) : QVariant());

    if (!query.exec()) {
        return fail(error, query, QObject::tr("Could not create the account."));
    }

    return true;
}

bool authenticate(const QString &email, const QString &password,
                  Account *account, QString *error)
{
    QSqlQuery query;
    query.prepare(QStringLiteral(
        "SELECT ID, email, passwordHash, firstName, lastName "
        "FROM tblAccount WHERE email = :email"));
    query.bindValue(QStringLiteral(":email"), email);

    if (!query.exec()) {
        return fail(error, query, QObject::tr("Could not read the account."));
    }

    // One message for an unknown address and for a wrong password: saying
    // which was wrong tells an attacker which addresses are registered.
    const QString rejected = QObject::tr("That email address and password do not match.");

    if (!query.next()) {
        setError(error, rejected);
        return false;
    }

    const int id = query.value(0).toInt();
    const QString storedHash = query.value(2).toString();

    if (!PasswordHasher::verify(password, storedHash)) {
        setError(error, rejected);
        return false;
    }

    if (account) {
        account->id = id;
        account->email = query.value(1).toString();
        account->firstName = query.value(3).toString();
        account->lastName = query.value(4).toString();
    }

    QSqlQuery stamp;
    if (PasswordHasher::needsRehash(storedHash)) {
        // The password is known correct here, so this is the only moment a
        // stronger hash can be written without asking for it again.
        const QString upgraded = PasswordHasher::hash(password);
        if (!upgraded.isEmpty()) {
            stamp.prepare(QStringLiteral(
                "UPDATE tblAccount SET lastSignInAt = CURRENT_TIMESTAMP, "
                "passwordHash = :hash WHERE ID = :id"));
            stamp.bindValue(QStringLiteral(":hash"), upgraded);
            stamp.bindValue(QStringLiteral(":id"), id);
            stamp.exec();
            return true;
        }
    }

    stamp.prepare(QStringLiteral(
        "UPDATE tblAccount SET lastSignInAt = CURRENT_TIMESTAMP WHERE ID = :id"));
    stamp.bindValue(QStringLiteral(":id"), id);
    stamp.exec();

    return true;
}

} // namespace AccountRepository
