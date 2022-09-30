#!/usr/bin/env python3

import sqlite3
import sys

DB_FILE = "static/ltype.db"
ASSETS_FILE = "include/assetsID.hpp"
CONST_FILE = "include/constants.hpp"


def searchAsset(searchedEntity, searchedNumber, searchedValue):
    with open(ASSETS_FILE, "r") as data:
        content = data.readlines()
        for line in content[2:]:
            if line.strip() != "":
                asset = line.strip().split(" ")[2].split("_")
                if asset[1] == searchedEntity and asset[2] == searchedNumber and asset[3] == searchedValue:
                    return line.split(" ")[4].strip()[:-1]


def build_levels(db, level_file_path):
    cur = db.cursor()
    cur.execute("INSERT INTO levels (creator) VALUES ('tijl')")
    cur.execute("SELECT last_insert_rowid()")
    lvlID = cur.fetchone()[0]
    db.commit()

    lvl_file = open(level_file_path, "r")
    content = lvl_file.readlines()
    lvl_file.close()

    for values in content:
        progress, name, xPos = values.split(",")
        splitted = name.split("_")
        entity = splitted[0]
        number = splitted[1]
        ID = searchAsset(entity, number, "ID")
        width = searchAsset(entity, number, "WIDTH")
        height = searchAsset(entity, number, "HEIGHT")
        values = [lvlID, progress, ID, xPos, width, height]
        cur.execute(f"INSERT INTO level_entities (level, progress, entity, xPos, yPos, xSize, ySize, xVelocity, yVelocity) VALUES (?, ?, ?, ?, 0, ?, ?, 0, 0)", values)
        db.commit()


if __name__ == "__main__":
    level_file_path = sys.argv[1]
    db = sqlite3.connect(DB_FILE)

    build_levels(db, level_file_path)

    db.close()
