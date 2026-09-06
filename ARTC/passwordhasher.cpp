#include "passwordhasher.h"

#include <QByteArray>
#include <QStringList>

#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <openssl/rand.h>

namespace {

// 64 MiB and three passes puts a single hash at roughly 150ms on a desktop
// CPU, the usual target for an interactive sign-in: slow enough to make
// offline cracking expensive, fast enough not to be felt.
constexpr quint32 MemoryCostKiB = 65536;
constexpr quint32 TimeCost = 3;
constexpr quint32 Lanes = 1;
constexpr quint32 Argon2Version = 19; // 0x13
constexpr int SaltBytes = 16;
constexpr int HashBytes = 32;

/** Base64 without padding, as the PHC string format specifies. */
QString b64(const QByteArray &raw)
{
    return QString::fromLatin1(raw.toBase64(QByteArray::Base64Encoding
                                            | QByteArray::OmitTrailingEquals));
}

QByteArray unb64(const QString &text)
{
    return QByteArray::fromBase64(text.toLatin1(), QByteArray::Base64Encoding);
}

/**
 * @brief Run Argon2id over @p password with the given cost.
 * @return the derived key, or an empty array if OpenSSL refused
 */
QByteArray derive(const QString &password, const QByteArray &salt,
                  quint32 memoryCostKiB, quint32 timeCost, quint32 lanes,
                  int outputBytes)
{
    EVP_KDF *kdf = EVP_KDF_fetch(nullptr, "ARGON2ID", nullptr);
    if (!kdf) {
        return QByteArray();
    }

    EVP_KDF_CTX *ctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);
    if (!ctx) {
        return QByteArray();
    }

    const QByteArray secret = password.toUtf8();
    quint32 threads = lanes;

    OSSL_PARAM params[7];
    params[0] = OSSL_PARAM_construct_octet_string(
        OSSL_KDF_PARAM_PASSWORD, const_cast<char *>(secret.constData()),
        static_cast<size_t>(secret.size()));
    params[1] = OSSL_PARAM_construct_octet_string(
        OSSL_KDF_PARAM_SALT, const_cast<char *>(salt.constData()),
        static_cast<size_t>(salt.size()));
    params[2] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES, &lanes);
    params[3] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threads);
    params[4] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST, &memoryCostKiB);
    params[5] = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ITER, &timeCost);
    params[6] = OSSL_PARAM_construct_end();

    QByteArray out(outputBytes, Qt::Uninitialized);
    const int rc = EVP_KDF_derive(ctx, reinterpret_cast<unsigned char *>(out.data()),
                                  static_cast<size_t>(outputBytes), params);
    EVP_KDF_CTX_free(ctx);

    return rc == 1 ? out : QByteArray();
}

/** One parsed PHC string. */
struct Parsed
{
    bool valid = false;
    quint32 memoryCostKiB = 0;
    quint32 timeCost = 0;
    quint32 lanes = 0;
    QByteArray salt;
    QByteArray hash;
};

/**
 * @brief Parse $argon2id$v=19$m=..,t=..,p=..$salt$hash.
 *
 * Anything that does not match exactly is reported invalid; callers treat
 * that as a failed verification rather than an error.
 */
Parsed parse(const QString &encoded)
{
    Parsed result;

    const QStringList fields = encoded.split(QLatin1Char('$'));
    // A leading '$' produces an empty first field.
    if (fields.size() != 6 || !fields.at(0).isEmpty()
        || fields.at(1) != QLatin1String("argon2id")) {
        return result;
    }

    if (fields.at(2) != QStringLiteral("v=%1").arg(Argon2Version)) {
        return result;
    }

    quint32 memoryCostKiB = 0;
    quint32 timeCost = 0;
    quint32 lanes = 0;
    for (const QString &pair : fields.at(3).split(QLatin1Char(','))) {
        const QStringList kv = pair.split(QLatin1Char('='));
        if (kv.size() != 2) {
            return result;
        }
        bool ok = false;
        const quint32 value = kv.at(1).toUInt(&ok);
        if (!ok) {
            return result;
        }
        if (kv.at(0) == QLatin1String("m")) {
            memoryCostKiB = value;
        } else if (kv.at(0) == QLatin1String("t")) {
            timeCost = value;
        } else if (kv.at(0) == QLatin1String("p")) {
            lanes = value;
        } else {
            return result;
        }
    }

    if (!memoryCostKiB || !timeCost || !lanes) {
        return result;
    }

    result.salt = unb64(fields.at(4));
    result.hash = unb64(fields.at(5));
    if (result.salt.isEmpty() || result.hash.isEmpty()) {
        return result;
    }

    result.memoryCostKiB = memoryCostKiB;
    result.timeCost = timeCost;
    result.lanes = lanes;
    result.valid = true;
    return result;
}

} // namespace

namespace PasswordHasher {

QString hash(const QString &password)
{
    QByteArray salt(SaltBytes, Qt::Uninitialized);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(salt.data()), SaltBytes) != 1) {
        return QString();
    }

    const QByteArray digest =
        derive(password, salt, MemoryCostKiB, TimeCost, Lanes, HashBytes);
    if (digest.isEmpty()) {
        return QString();
    }

    return QStringLiteral("$argon2id$v=%1$m=%2,t=%3,p=%4$%5$%6")
        .arg(Argon2Version)
        .arg(MemoryCostKiB)
        .arg(TimeCost)
        .arg(Lanes)
        .arg(b64(salt), b64(digest));
}

bool verify(const QString &password, const QString &encoded)
{
    const Parsed stored = parse(encoded);
    if (!stored.valid) {
        return false;
    }

    const QByteArray candidate = derive(password, stored.salt, stored.memoryCostKiB,
                                        stored.timeCost, stored.lanes,
                                        stored.hash.size());
    if (candidate.size() != stored.hash.size()) {
        return false;
    }

    // Constant time: a byte-by-byte comparison leaks how much of the hash
    // matched, which is enough to reconstruct it one byte at a time.
    return CRYPTO_memcmp(candidate.constData(), stored.hash.constData(),
                         static_cast<size_t>(stored.hash.size())) == 0;
}

bool needsRehash(const QString &encoded)
{
    const Parsed stored = parse(encoded);
    if (!stored.valid) {
        return true;
    }
    return stored.memoryCostKiB < MemoryCostKiB
           || stored.timeCost < TimeCost
           || stored.lanes != Lanes;
}

} // namespace PasswordHasher
