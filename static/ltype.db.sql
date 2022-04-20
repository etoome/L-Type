-- Accounts --

DROP TABLE IF EXISTS 'accounts';
CREATE TABLE IF NOT EXISTS 'accounts' (
	'username'	TEXT PRIMARY KEY,
	'password'	TEXT NOT NULL,
	'admin'	INTEGER NOT NULL DEFAULT 0
);
INSERT INTO 'accounts' ('username', 'password', 'admin') VALUES ('tijl', '_', 1);
-- admin:password
INSERT INTO 'accounts' ('username', 'password', 'admin') VALUES ('admin', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 1);

-- Follows --

DROP TABLE IF EXISTS 'follow';
CREATE TABLE IF NOT EXISTS 'follow' (
	'follower'	TEXT NOT NULL,
	'followed'	TEXT NOT NULL,
	FOREIGN KEY('follower') REFERENCES 'accounts'('username'),
	FOREIGN KEY('followed') REFERENCES 'accounts'('username')
);

-- Leaderboard --

DROP TABLE IF EXISTS 'leaderboard';
CREATE TABLE IF NOT EXISTS 'leaderboard' (
	'username'	TEXT NOT NULL UNIQUE,
	'best_score'	INTEGER NOT NULL DEFAULT 0,
	'xp'	INTEGER NOT NULL DEFAULT 0,
	FOREIGN KEY('username') REFERENCES 'accounts'('username')
);

-- Pack --

DROP TABLE IF EXISTS 'pack';
CREATE TABLE IF NOT EXISTS 'pack' (
	'id'	INTEGER PRIMARY KEY,
	'name'	TEXT NOT NULL UNIQUE
);
DROP TABLE IF EXISTS 'account_pack';
CREATE TABLE IF NOT EXISTS 'account_pack' (
	'account'	TEXT NOT NULL,
	'pack'	INTEGER NOT NULL,
	FOREIGN KEY('account') REFERENCES 'accounts'('username'),
	FOREIGN KEY('pack') REFERENCES 'pack'('id')
);
DROP TABLE IF EXISTS 'pack_key';
CREATE TABLE IF NOT EXISTS 'pack_key' (
	'pack'	INTEGER NOT NULL,
	'key'	TEXT NOT NULL UNIQUE,
	'uses' INTEGER NOT NULL,
	FOREIGN KEY('pack') REFERENCES 'pack'('id')
);
INSERT INTO 'pack' ('id', 'name') VALUES ('1', 'Moinecroft');

-- Levels --

DROP TABLE IF EXISTS 'level_entities';
CREATE TABLE IF NOT EXISTS 'level_entities' (
	'id' INTEGER PRIMARY KEY AUTOINCREMENT,
	'level' INTEGER NOT NULL,
	'progress' INTEGER NOT NULL,
	'entity' INTEGER NOT NULL,
	'xPos' INTEGER NOT NULL,
	'yPos' INTEGER NOT NULL DEFAULT 0,
	'xSize' INTEGER NOT NULL,
	'ySize' INTEGER NOT NULL,
	'xVelocity' INTEGER NOT NULL,
	'yVelocity' INTEGER NOT NULL,
	FOREIGN KEY('level') REFERENCES 'levels'('id')
);
DROP TABLE IF EXISTS 'levels';
CREATE TABLE IF NOT EXISTS 'levels' (
	'id'	INTEGER PRIMARY KEY AUTOINCREMENT,
	'date' TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	'name' TEXT NOT NULL DEFAULT "",
	'creator'	TEXT NOT NULL,
	FOREIGN KEY('creator') REFERENCES 'accounts'('username')
);
DROP TABLE IF EXISTS 'level_rating';
CREATE TABLE IF NOT EXISTS 'level_rating' (
	'level'	INTEGER NOT NULL,
	'user'	TEXT NOT NULL,
	'rate' INTEGER NOT NULL,
	PRIMARY KEY ('level', 'user'),
	FOREIGN KEY('level') REFERENCES 'levels'('id'),
	FOREIGN KEY('user') REFERENCES 'accounts'('username')
);
