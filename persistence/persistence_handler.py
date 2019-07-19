import psycopg2
from persistence import config as config
import time

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
