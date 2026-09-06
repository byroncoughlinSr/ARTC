-- Deploy artc:child to mysql
-- requires: person
-- idempotent

--
-- The parent-to-child edge, written by DatabaseHelper::addChild.
--
-- This duplicates what tblPerson.fatherId/motherId already record, and no
-- code reads it back. It is created because the application writes to it;
-- the unique key at least stops the same edge being inserted twice.
--

CREATE TABLE IF NOT EXISTS tblChild (
    ID       INT UNSIGNED NOT NULL AUTO_INCREMENT,
    parentId INT UNSIGNED NOT NULL,
    childId  INT UNSIGNED NOT NULL,
    PRIMARY KEY (ID),
    UNIQUE KEY uq_child_edge (parentId, childId),
    KEY idx_child_child (childId),
    CONSTRAINT fk_child_parent FOREIGN KEY (parentId)
        REFERENCES tblPerson (ID) ON DELETE CASCADE,
    CONSTRAINT fk_child_child FOREIGN KEY (childId)
        REFERENCES tblPerson (ID) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
