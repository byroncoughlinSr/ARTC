-- Revert artc:person_death from mysql

ALTER TABLE tblPerson DROP COLUMN deathdate;
