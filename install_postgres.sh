#!/usr/bin/env bash
dropdb --if-exists "Viri2" && createdb "Viri2" && psql -d "Viri2" -c "DROP TABLE IF EXISTS utente, file CASCADE;" -c "CREATE TABLE utente (username varchar(20) NOT NULL PRIMARY KEY, nome varchar(20) NOT NULL, cognome varchar(20) NOT NULL,
flag integer, password varchar(255) NOT NULL);" -c "CREATE TABLE file (nex varchar(255) NOT NULL PRIMARY KEY, output_file varchar(255) NOT NULL, utente varchar(20) NOT NULL
REFERENCES utente(username), descrizione varchar(1500), input_path varchar(255) NOT NULL, output_path varchar(255) NOT NULL);"