-- Verify artc:account on mysql

SELECT ID, email, passwordHash, firstName, middleName, lastName,
       birthdate, personId, createdAt, lastSignInAt
FROM tblAccount WHERE 1 = 0;
