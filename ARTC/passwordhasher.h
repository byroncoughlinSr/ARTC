#ifndef PASSWORDHASHER_H
#define PASSWORDHASHER_H

#include <QString>

/**
 * @brief Argon2id password hashing, via OpenSSL's KDF.
 *
 * Hashes are returned in PHC string format, which carries the algorithm and
 * its cost parameters alongside the salt and digest:
 *
 *     $argon2id$v=19$m=65536,t=3,p=1$<salt>$<hash>
 *
 * Because the cost travels with the hash, verify() can check a password
 * against an older, cheaper hash while needsRehash() reports that it should be
 * replaced the next time the owner signs in successfully.
 */
namespace PasswordHasher {

/**
 * @brief Hash @p password with the current cost parameters and a fresh salt.
 * @return a PHC string, or an empty string if the platform's RNG or KDF failed
 */
QString hash(const QString &password);

/**
 * @brief Check @p password against a stored PHC string.
 *
 * The comparison is constant-time. A malformed or unsupported @p encoded
 * value verifies as false rather than throwing.
 */
bool verify(const QString &password, const QString &encoded);

/**
 * @brief Whether @p encoded was produced below the current cost policy.
 *
 * True also for a hash this build cannot parse, so that unknown formats are
 * replaced rather than kept.
 */
bool needsRehash(const QString &encoded);

} // namespace PasswordHasher

#endif // PASSWORDHASHER_H
