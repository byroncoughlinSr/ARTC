-- Verify artc:person on mysql
-- Fails if the table or any column the application uses is missing.

SELECT ID, firstName, middleName, lastName, gender, birthdate, fatherId, motherId
FROM tblPerson WHERE 1 = 0;
