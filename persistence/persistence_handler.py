import psycopg2
from persistence import config as config

import time

#Funzion che connettendosi a un database salva un'istanza di utente
def save_user(first_name,last_name, username, pwd):
    conn = None
    flag = 0
    try:
        '''params = config.config()'''
        print("connecting to db")
        conn = psycopg2.connect(user="chiello",
                                  password="postgres",
                                  host="127.0.0.1",
                                  port="5432",
                                  database="Viri")
        print("connesso")
        cur = conn.cursor()
        cur.execute("SELECT * from Utente where username = %s", (username,))
        record = cur.fetchall()
        if record == []:
            postgres_insert_user = """INSERT INTO Utente (nome, username, password, cognome) VALUES (%s, %s, %s, %s)"""
            print(postgres_insert_user)
            record_to_insert = (first_name, username, pwd, last_name)
            print(record_to_insert)
            cur.execute(postgres_insert_user, record_to_insert)
            conn.commit()
            flag = 1
        else:
            flag = 2
    except(Exception, psycopg2.Error) as error:
        if(conn):
            return (error)

    finally:
        if(conn):
            cur.close()
            conn.close()
            print("connection closed")
            return flag

#Passando username path dei file input e output creo una persistenza per i file
def save_files_path(input_path, output_path, username):
    conn = None
    flag = 0
    try:
        '''params = config.config()'''
        print("connecting to db")
        conn = psycopg2.connect(user="chiello",
                                  password="postgres",
                                  host="127.0.0.1",
                                  port="5432",
                                  database="Viri")
        print("connesso")
        cur = conn.cursor()
        cur.execute("SELECT id from Utente where username = %s", (username,))
        user_id = cur.fetchone()
        record_to_insert = (input_path, output_path, user_id)
        cur.execute("INSERT INTO File (input_path, output_path, utente) VALUES (%s, %s, %s)", record_to_insert)
        conn.commit()
        flag = 1
    except(Exception, psycopg2.Error) as error:
        if(conn):
            return (error)
    finally:
        if(conn):
            cur.close()
            conn.close()
            print("connection closed")
            return flag

#Funzione che connettendosi al db restituisce la password corrispondente all'utente
def search_user_pwd(user):
    conn = None
    dati = []
    try:
        '''params = config.config()'''
        print("connecting to db")
        conn = psycopg2.connect(user="chiello",
                                  password="postgres",
                                  host="127.0.0.1",
                                  port="5432",
                                  database="Viri")
        print("connesso")
        cur = conn.cursor()
        cur.execute("SELECT password, flag from Utente where username = %s", (user,))
        dati = cur.fetchone()
    except(Exception, psycopg2.Error) as error:
        if(conn):
            return (error)

    finally:
        if(conn):
            cur.close()
            conn.close()
            print("connection closed")
            return dati

#Funzione che connettendosi al db restituisce l'username
def find_user(username):
    conn = None
    dati = None
    try:
        '''params = config.config()'''
        print("connecting to db")
        conn = psycopg2.connect(user="chiello",
                                password="postgres",
                                host="127.0.0.1",
                                port="5432",
                                database="Viri")
        print("connesso")
        cur = conn.cursor()
        cur.execute("SELECT username from Utente where username = %s", (username,))
        dati = cur.fetchone()
    except(Exception, psycopg2.Error) as error:
        if(conn):
            return (error)
    finally:
        if(conn):
            cur.close()
            conn.close()
            print("Connection closed")
            return dati