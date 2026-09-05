import mysql.connector

CONFIG_FILE = "config.ini"

db = mysql.connector.connect(option_files=CONFIG_FILE, option_groups=["connector_artc"])

print(db)
myCursor = db.cursor()

myCursor.execute("SHOW DATABASES")

for x in myCursor:
    print(x)
