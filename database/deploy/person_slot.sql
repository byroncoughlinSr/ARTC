-- Deploy artc:person_slot to mysql
-- requires: person
-- idempotent

--
-- Separate a slot's identity from the person who fills it.
--
-- The generator used to write its slot code ("MGF201-7") into firstName, so
-- the moment a user typed a real name over it the slot's identity was gone and
-- there was no way to empty the slot again. slotCode now holds that code for
-- the life of the row, and the name columns hold a person or nothing.
--
-- An empty slot is therefore "firstName IS NULL", which is why the name
-- columns become nullable here.
--

SET @has_slot := (
    SELECT COUNT(*) FROM information_schema.columns
    WHERE table_schema = DATABASE() AND table_name = 'tblPerson' AND column_name = 'slotCode');

SET @sql := IF(@has_slot = 0,
    'ALTER TABLE tblPerson ADD COLUMN slotCode VARCHAR(32) NULL AFTER ID,
       ADD KEY idx_person_slot (slotCode)',
    'DO 0');
PREPARE stmt FROM @sql; EXECUTE stmt; DEALLOCATE PREPARE stmt;

ALTER TABLE tblPerson
    MODIFY firstName VARCHAR(100) NULL,
    MODIFY lastName  VARCHAR(100) NULL;

-- Move any existing generated name into slotCode and empty the slot.
UPDATE tblPerson
   SET slotCode = firstName
 WHERE slotCode IS NULL
   AND (firstName IN ('FATHER', 'MOTHER')
        OR firstName REGEXP '^[FM]G[FM][0-9]+-[0-9]+$');

UPDATE tblPerson
   SET firstName = NULL, middleName = NULL, lastName = NULL, birthdate = NULL
 WHERE slotCode IS NOT NULL
   AND firstName = slotCode;
