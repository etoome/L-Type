#!/usr/bin/env python3

from os import system, name
import sys
import sqlite3
from sqlite3 import Error

DB_PATH = './static/ltype.db'


class Database:
    def __init__(self):
        try:
            self._db = sqlite3.connect(DB_PATH)
        except Error as e:
            print(e)

    def __del__(self):
        self._db.close()

    def createAdmin(self, username: str, password: str):
        if len(password) == 0:
            return

        cursor = self._db.execute(
            "INSERT OR REPLACE INTO 'accounts' ('username', 'password', 'admin') VALUES (?, ?, 1);", [username, password])
        self._db.commit()
        if self._db.total_changes == 0:
            print(f'Error')
        else:
            print(f'Account created')

    def listAdmin(self):
        cursor = self._db.execute(
            "SELECT username FROM accounts WHERE admin = 1")
        i = 0
        for row in cursor:
            i += 1
            print(f'{i}. {row[0]}')

        if i == 0:
            print(f'No admin')

    def updateAdmin(self, username: str, state: bool):
        if len(username) == 0:
            return

        cursor = self._db.execute(
            "UPDATE accounts SET admin = ? WHERE username = ?", [1 if state else 0, username])
        self._db.commit()
        if self._db.total_changes == 0:
            print(f'User not found')
        else:
            if state:
                print(f'Admin role added to {username}')
            else:
                print(f'Admin role removed from {username}')


def clear():
    if name == 'nt':
        system('cls')
    else:
        system('clear')


def menu():
    printMenu()

    close = False
    while not close:
        opt = input("\n> ").strip()
        clear()
        print("")
        if opt == "":
            printMenu()
        elif opt == "1":
            db.listAdmin()
        elif opt == "2":
            username = input(f'Username : ')
            db.updateAdmin(username, True)
        elif opt == "3":
            username = input(f'Username : ')
            db.updateAdmin(username, False)
        elif opt == "4":
            username = input(f'Username : ')
            password = input(f'Password : ')
            db.createAdmin(username, password)
        elif opt == "0":
            close = True


def printMenu():
    clear()
    print("""
1 : List admin
2 : Add
3 : Remove
4 : Create
0 : Exit""")


def main():
    db = Database()

    if len(sys.argv) == 3 and sys.argv[1] == "add":
        db.updateAdmin(sys.argv[2], True)
    elif len(sys.argv) == 3 and sys.argv[1] == "remove":
        db.updateAdmin(sys.argv[2], False)
    elif len(sys.argv) == 4 and sys.argv[1] == "create":
        db.createAdmin(sys.argv[2], sys.argv[3])
    else:
        menu()

    del db


main()
