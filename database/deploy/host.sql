-- Deploy artc:host to mysql
-- requires: person
-- idempotent

--
-- The root person of a pedigree: the individual every DNA match relates to.
-- Written by DatabaseHelper::setHost.
--

CREATE TABLE IF NOT EXISTS tblHost (
    ID       INT UNSIGNED NOT NULL AUTO_INCREMENT,
    personId INT UNSIGNED NOT NULL,
    PRIMARY KEY (ID),
    UNIQUE KEY uq_host_person (personId),
    CONSTRAINT fk_host_person FOREIGN KEY (personId)
        REFERENCES tblPerson (ID) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
