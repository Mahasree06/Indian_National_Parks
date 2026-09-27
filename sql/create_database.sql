DROP TABLE IF EXISTS national_parks;

CREATE TABLE national_parks
(
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    park_name TEXT NOT NULL,

    state_ut TEXT NOT NULL,

    location TEXT,

    formed_year TEXT,

    notable_features TEXT,

    flora_fauna TEXT,

    rivers_lakes TEXT
);
