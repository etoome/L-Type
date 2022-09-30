#!/usr/bin/env python3

import sqlite3
import os.path


DB_FILE = "static/ltype.db"
DB_SCHEMA = "static/ltype.db.sql"


def build_database():
    db = sqlite3.connect(DB_FILE)
    cur = db.cursor()

    schema_file = open(DB_SCHEMA, "r")

    line = schema_file.readline()
    while line:
        schemaStr = ""
        while schemaStr == "" or schemaStr[-2] != ";":
            if (line != "\n" and line[0:2] != "--"):
                schemaStr += line
            line = schema_file.readline()
        cur.execute(schemaStr[:-1])

    db.commit()
    db.close()


if __name__ == "__main__":
    if (not os.path.isfile(DB_FILE)):
        build_database()
