-- Deploy artc:person to mysql
-- idempotent

--
-- Every individual the system knows about: the host, real relatives, and the
-- placeholder ancestors the pedigree generator creates.
--
-- fatherId and motherId form a self-referencing adjacency list. Both are
-- nullable, because a person whose parents have not been generated yet has
-- neither, and ON DELETE SET NULL keeps the tree readable if one is removed.
--
-- middleName, gender and birthdate are nullable: DatabaseHelper::addMother and
-- addFather insert placeholders with only a first and last name.
--

CREATE TABLE IF NOT EXISTS tblPerson (
    ID         INT UNSIGNED NOT NULL AUTO_INCREMENT,
    firstName  VARCHAR(100) NOT NULL,
    middleName VARCHAR(100) NULL,
    lastName   VARCHAR(100) NOT NULL,
    gender     CHAR(1) NULL,
    birthdate  DATE NULL,
    fatherId   INT UNSIGNED NULL,
    motherId   INT UNSIGNED NULL,
    PRIMARY KEY (ID),
    -- Serves getPersonId (first+middle+last) and, on its leading columns,
    -- getFatherId/getMotherId (first+last).
    KEY idx_person_name (firstName, lastName, middleName),
    KEY idx_person_father (fatherId),
    KEY idx_person_mother (motherId),
    CONSTRAINT fk_person_father FOREIGN KEY (fatherId)
        REFERENCES tblPerson (ID) ON DELETE SET NULL,
    CONSTRAINT fk_person_mother FOREIGN KEY (motherId)
        REFERENCES tblPerson (ID) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
