-- Deploy artc:person_death to mysql
-- requires: person
-- idempotent

--
-- A death date alongside the birth date. Person::Individual has carried a
-- deathdate field since 2018 but the table never had a column for it, so it
-- was silently dropped on every write.
--

SET @has_death := (
    SELECT COUNT(*) FROM information_schema.columns
    WHERE table_schema = DATABASE() AND table_name = 'tblPerson'
      AND column_name = 'deathdate');

SET @sql := IF(@has_death = 0,
    'ALTER TABLE tblPerson ADD COLUMN deathdate DATE NULL AFTER birthdate',
    'DO 0');
PREPARE stmt FROM @sql; EXECUTE stmt; DEALLOCATE PREPARE stmt;
