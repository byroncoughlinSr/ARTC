-- Deploy artc:account to mysql
-- requires: person
-- idempotent

--
-- A sign-in account.
--
-- passwordHash holds a PHC-format Argon2id string, which carries its own
-- parameters:
--
--   $argon2id$v=19$m=65536,t=3,p=1$<salt>$<hash>
--
-- Storing the cost inline means the work factor can be raised later without
-- invalidating existing hashes: PasswordHasher::needsRehash spots a hash below
-- current policy and it is replaced on the owner's next successful sign-in.
--
-- personId is the account holder's own row in tblPerson, once they create a
-- host. It is null until then: registration deliberately does not generate a
-- pedigree, so signing up and building a chart stay separate steps.
--

CREATE TABLE IF NOT EXISTS tblAccount (
    ID           INT UNSIGNED NOT NULL AUTO_INCREMENT,
    email        VARCHAR(254) NOT NULL,
    passwordHash VARCHAR(255) NOT NULL,
    firstName    VARCHAR(100) NOT NULL,
    middleName   VARCHAR(100) NULL,
    lastName     VARCHAR(100) NOT NULL,
    birthdate    DATE NULL,
    personId     INT UNSIGNED NULL,
    createdAt    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    lastSignInAt TIMESTAMP NULL DEFAULT NULL,
    PRIMARY KEY (ID),
    -- Case-insensitive: the collation makes 'A@b.com' and 'a@b.com' one account.
    UNIQUE KEY uq_account_email (email),
    KEY idx_account_person (personId),
    CONSTRAINT fk_account_person FOREIGN KEY (personId)
        REFERENCES tblPerson (ID) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
