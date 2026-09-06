#ifndef ACCOUNTREPOSITORY_H
#define ACCOUNTREPOSITORY_H

#include <QString>

struct RegistrationDetails;

/**
 * @brief One row of tblAccount, without the password hash.
 */
struct Account
{
    int id = 0;
    QString email;
    QString firstName;
    QString lastName;
};

/**
 * @brief Reads and writes tblAccount.
 *
 * Every statement here is prepared and bound. DatabaseHelper builds several of
 * its queries by concatenating strings; that must not spread to a table which
 * holds credentials and is fed directly from a login form.
 *
 * The caller is responsible for having opened the database first.
 */
namespace AccountRepository {

/** @brief Whether an account already exists for @p email (case-insensitive). */
bool emailTaken(const QString &email, QString *error = nullptr);

/**
 * @brief Create an account from @p details, hashing the password.
 * @param[out] error a message suitable for showing to the user
 * @return true when the row was inserted
 */
bool create(const RegistrationDetails &details, QString *error = nullptr);

/**
 * @brief Verify @p email and @p password against the stored hash.
 *
 * On success, stamps lastSignInAt and transparently upgrades a hash that is
 * below the current cost policy.
 *
 * @param[out] account filled in on success
 * @param[out] error a message suitable for showing to the user
 * @return true when the credentials matched
 */
bool authenticate(const QString &email, const QString &password,
                  Account *account, QString *error = nullptr);

} // namespace AccountRepository

#endif // ACCOUNTREPOSITORY_H
