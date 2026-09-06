#!/usr/bin/env python3
"""
Create the ARTC database and its application account, then record the
connection settings in config.ini.

Run once, as an administrator. Running it again is harmless: it re-applies the
grants and rewrites the password.

    ./initial_setup.py                    # sudo to the local MySQL root
    ./initial_setup.py -u root -p secret  # or connect with a password
    ./initial_setup.py --dropdb           # recreate from scratch, losing data
"""

import argparse
import sys
from secrets import token_urlsafe

from utils import (
    CONFIG_SECTION,
    DEFAULT_DATABASE,
    mysql_command,
    read_config,
    write_config,
)


def scalar(admin, sql):
    """Run a query and return its single value, or None."""
    return (mysql_command(sql=sql, **admin) or "").strip() or None


def database_exists(admin, database):
    return scalar(
        admin,
        "SELECT schema_name FROM information_schema.schemata "
        f"WHERE schema_name = '{database}'",
    ) == database


def user_exists(admin, user, host):
    return scalar(
        admin,
        f"SELECT user FROM mysql.user WHERE user = '{user}' AND host = '{host}'",
    ) == user


def main():
    config = read_config()
    section = config[CONFIG_SECTION]

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--database", default=section.get("database", DEFAULT_DATABASE),
                        help="name of the database to create")
    parser.add_argument("--app-user", default=section.get("user", "artc"),
                        help="the account the application signs in as")
    parser.add_argument("--app-host", default="localhost",
                        help="the host part of the application account")
    parser.add_argument("--app-password", default=None,
                        help="password for the application account (generated if omitted)")
    parser.add_argument("--host", default=section.get("host", "localhost"),
                        help="the MySQL server host")
    parser.add_argument("--port", default=int(section.get("port", 3306)), type=int,
                        help="the MySQL server port")
    parser.add_argument("-u", "--user", default=None,
                        help="an administrative MySQL account")
    parser.add_argument("-p", "--password", default=None,
                        help="password for the administrative account")
    parser.add_argument("--sudo", action="store_true", default=None,
                        help="reach the administrative account with sudo (the default)")
    parser.add_argument("--no-sudo", dest="sudo", action="store_false",
                        help="connect as --user instead of using sudo")
    parser.add_argument("--dropdb", action="store_true",
                        help="drop the database first, destroying all data")
    parser.add_argument("-d", "--debug", action="store_true", help="echo commands")
    args = parser.parse_args()

    use_sudo = args.sudo if args.sudo is not None else args.user is None
    admin = dict(user=args.user, password=args.password, host=args.host,
                 port=args.port, sudo=use_sudo, debug=args.debug)

    try:
        scalar(admin, "SELECT 1")
    except RuntimeError as error:
        print(f"Could not connect as an administrator: {error}", file=sys.stderr)
        print("Try ./initial_setup.py -u root -p <password> --no-sudo", file=sys.stderr)
        return 1

    database = args.database
    if args.dropdb and database_exists(admin, database):
        print(f"Dropping database {database}")
        mysql_command(sql=f"DROP DATABASE {database}", **admin)

    mysql_command(
        sql=f"CREATE DATABASE IF NOT EXISTS {database} "
            "CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci",
        **admin,
    )
    print(f"Database {database} ready")

    app_user, app_host = args.app_user, args.app_host
    password = args.app_password or section.get("password") or token_urlsafe(18)
    existed = user_exists(admin, app_user, app_host)

    # ALTER rather than CREATE-only, so an account left over from an older
    # server is moved onto caching_sha2_password. An account still carrying a
    # mysql_native_password hash cannot authenticate against MySQL 8.4, which
    # no longer loads that plugin by default.
    mysql_command(
        sql=f"CREATE USER IF NOT EXISTS '{app_user}'@'{app_host}' "
            f"IDENTIFIED WITH caching_sha2_password BY '{password}'",
        **admin,
    )
    mysql_command(
        sql=f"ALTER USER '{app_user}'@'{app_host}' "
            f"IDENTIFIED WITH caching_sha2_password BY '{password}'",
        **admin,
    )
    mysql_command(
        sql=f"GRANT ALL PRIVILEGES ON {database}.* TO '{app_user}'@'{app_host}'",
        **admin,
    )
    mysql_command(sql="FLUSH PRIVILEGES", **admin)
    print(f"Account '{app_user}'@'{app_host}' {'updated' if existed else 'created'}")

    section["host"] = args.host
    section["port"] = str(args.port)
    section["database"] = database
    section["user"] = app_user
    section["password"] = password
    path = write_config(config)
    print(f"Wrote {path} (mode 600)")
    print("\nNext: ./db_updater.py deploy")
    return 0


if __name__ == "__main__":
    sys.exit(main())
