import psycopg2
from persistence import config as config

def save_user(first_name, last_name, username, password):
    conn = None

    try:
        params = config.config()
        print("connecting to db")

        conn = psycopg2.connect(**params)
        cur = conn.cursor()
        postgres_insert_user = """INSERT into Utente
        (nome, username, password, cognome) VALUES (%s, %s, %s %s)"""
        record_to_insert = ('a', 'b', 'c', 'd')
        cur.execute(postgres_insert_user, record_to_insert)
        conn.commit()
        count = cur.rowcount
        print(count, "Record inserted succesfully")
        return True
    except(Exception, psycopg2.Error) as error:
        if(conn):
            return (error)

    finally:
        if(conn):
            cur.close()
            conn.close()
            print("connection closed")
