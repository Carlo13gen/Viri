
1) INSTALLAZIONE
================

1.1) Installazione e configurazione di postgresql
-------------------------------------------------

Postgresql va installato come descritto in questa sezione *prima* di perfezionare l'installazione di python3 come è descritto nella sezione 1.2, in quanto ci sono dei pacchetti di python3 (specificatamente psycopg2) che presuppongono l'esistenza di posgresql.

1.1.1) Verificare che il package postgresql sia installato sulla propria macchina. Eventualmente installarlo ("sudo apt-get install postgresql"). L'installazione di "postgresql" comporta l'installazione di "postgresql-x.y", "postgresql-common", "postgresql-contrib-x.y").

1.1.2) Installare il package libqp-dev ("sudo apt-get install libpq-dev"). Questo pacchetto servirà al pacchetto python3 psycopg2. Quando si installa questo pacchetto viene suggerita l'installazione anche di postgresql-doc-X.Y (per esempio: postgresql-doc-9.6).

1.1.3) Configurazione di postgresql e creazione delle tabelle necessarie:

1.1.3.1) Deve essere presente un account postgresql con il nome del vostro userid linux.
Per verificare che l'account sportaGDTEucalipt.py sia già presente provare ad eseguire il passo seguente 1.1.3.2 e se si riceve l'errore:
> dropdb: could not connect to database template1: FATAL:  role "patrigna" does not exist
allora occorre creare un account postgresql con il proprio userid linux come descritto qui sotto.
eseguire: "su - postgres" (questo vi fa diventare l'utente linux "postgres" con tutto il suo ambiente) 
poi eseguire: "createuser --interactive --pwprompt"
	Enter name of role to add: patrigna
	Enter password for new role: xxxxxxx
	Enter it again: xxxxxxx
	Shall the new role be a superuser? (y/n) n
	Shall the new role be allowed to create databases? (y/n) y
	Shall the new role be allowed to create more new roles? (y/n) y

1.1.3.2) Lanciare lo script install_postgres.sh. 
Attenzione: questo script cancella il database Viri se già esistente.
Si riceve un output di questo tenore:
	NOTICE:  database "Viri" does not exist, skipping
	NOTICE:  table "utente" does not exist, skipping
	NOTICE:  table "file" does not exist, skipping
	DROP TABLE
	CREATE TABLE
	CREATE TABLE
(le prime tre righe non sono un errore, sono il tentativo di cancellare il precedente database che in realtà è assente).
 
1.1.4) Per testare che la creazione delle tabelle sia avvenuta correttamente lanciare lo script "test_postgres.sh"
Viene prodotto un output come segue:
             Table "public.utente"
  Column  |          Type          | Modifiers 
----------+------------------------+-----------
 username | character varying(20)  | not null
 nome     | character varying(20)  | not null
 cognome  | character varying(20)  | not null
 flag     | integer                | 
 password | character varying(255) | not null
Indexes:
    "utente_pkey" PRIMARY KEY, btree (username)
Referenced by:
    TABLE "file" CONSTRAINT "file_utente_fkey" FOREIGN KEY (utente) REFERENCES utente(username)

                Table "public.file"
   Column    |          Type           | Modifiers 
-------------+-------------------------+-----------
 nex         | character varying(255)  | not null
 output_file | character varying(255)  | not null
 utente      | character varying(20)   | not null
 descrizione | character varying(1500) | 
 input_path  | character varying(255)  | not null
 output_path | character varying(255)  | not null
Indexes:
    "file_pkey" PRIMARY KEY, btree (nex)
Foreign-key constraints:
    "file_utente_fkey" FOREIGN KEY (utente) REFERENCES utente(username)

1.1.5) Se si intende modificare le configurazioni di accesso al database Viri su postgres: 
Dall'utente in cui ci si trova digitare sul terminale: 
psql -d Viri
Dopodichè il comando: ALTER USER "nome_utente" with password "nuova_password";
In questo modo la password per "nome_utente" è stata cambiata in "nuova_password"
(Digitare \q per uscire da psql).

1.1.6) Modificare il file /persistance/persistance_handler.py
Settare nelle prime righe del file le seguenti stringhe: 
	USER = "nome_utente"
    PASSWORD = "nuova_password"
    HOST = "127.0.0.1"
    PORT = "5432"
    DATABASE = "Viri"
Dove "nome_utente" e "nuova_password" sono l'utente e la password scelta per il database Viri su postgres.

1.2) Installazioni relative a python3
-------------------------------------

1.2.1) Installare pip3 ("sudo apt-get install pip3")

1.2.2) installare flask ("pip3 install flask")
Viene installato anche Jinja2, itsdangerous, click, Werkzeug, MarkupSafe

1.2.3) installare flask_cors ("pip3 install flask_cors")
Viene installato anche Six, click, MarkupSafe, Jinja2, Werkzeug, itsdangerous, Flask

1.2.4) Installare flask_bcrypt ("pip3 install flask_bcrypt")
Vengono installat anche MarkupSafe, Jinja2, click, itsdangerous, Werkzeug, Flask, six, pycparser, cffi, bcrypt

1.2.5) Installare psycopg2 ("pip3 install psycopg2")

Error: b'You need to install postgresql-server-dev-X.Y for building a server-side extension or libpq-dev for building a client-side application.\n'
   postgresql-doc-9.6

1.2.3) installare networkx versione 1 ("pip3 install networkx==1.11"). 
Con networkx viene installato anche decorator.
Attenzione alla versione che non deve essere 2.x. 
Se è già installato networkx versione 2.x (si vede facendo "pip3 list | grep networkx") occorre fare un downgrade eseguendo in sequenza i due comandi "pip3 uninstall networkx -y" e "pip3 install networkx==1.11".

1.2.4) installare lxml ("pip3 install lxml")

1.2.5) installare psycopg2 ("pip3 install psycopg2").
Potrebbe verificarsi il seguente errore:
Error: b'You need to install postgresql-server-dev-X.Y for building a server-side extension or libpq-dev for building a client-side application.\n'

1.2.6) installare flask_login ("pip3 install flask_login").
Vengono installati (se non presenti) anche: itsdangerous, MarkupSafe, Jinja2, Werkzeug, click, Flask

2) LANCIO
=========

2.1) Cambiare la variabile di sistema $TERM nel terminale installato sulla vostra macchina. Per esempio "TERM=gnome-terminal", oppure "TERM=xterm", oppure "TERM=konsole".

2.2) Eseguire ./avvio.sh  (si aprono 4 finestrelle)

2.3) Aprire un browser e connettersi ad http://localhost:5000

2.4) Popolazione del database. 
La prima volta che si fa partire Viri non c'è nulla nel database (né utenti, né alberi co-filogenetici).
La prima operazione da fare è creare un utente. In questo momento Viri non prevede un utente amministratore con privilegi particolari. Tuttavia si suggerisce di creare un utente "Administrator" e di usare questo per caricare i primi file.
Per caricare i file seguire le istruzioni della pagina di caricamento che si apre selezionando UPLOAD dal menu. 

3) SHUTDOWN
===========

3.1) Eseguire ./shutdown.sh  (vengono killate le 4 finestrelle e relativi processi)

4) DEBUG
========

4.1) Debug del dialogo tra browser e server(s)
----------------------------------------------

http://localhost:1998/graphs_output_gdt/PP_Administrator
In realtà /graphs_output_gdt/<nome> non corrisponde ad una directory ma al metodo delegato al calcolo del grafo che si trova nello script python “portaGDTEucalyptPreCalcolati.py”.  

Per vedere l'output del calcolo delle euristiche in python connettiti con il browser a:
http://localhost:9000/graphs_gdt/GL/-1/1/1/1/1/true/false/1/false/NaN

http://localhost:5000/test
Per lanciare una suite di test

http://localhost:8000/list_of_file
Ritorna la lista dei file su cui vengono fatti i test
Esempio di output: ["caryophyllaceae-microbotryum", "PML", "FD", "COG4965", "PMP", "stinkbugs-bacteria", "COG3715", "GL", "RH", "SFC", "EC", "COG2085", "SC", "COG4964", "PP", "RP"]
Praticamente si prende tutti i file della cartella "input_data"

http://localhost:8000/list_of_output_test
Mostra i file output dei test
Esempio di output (quando non sono stati effettuati test): []

4.2 Debug di postgresql
-----------------------

4.2.1 Vedere gli utenti di Viri
lanciare: psql -d "Viri"
Viri=> select * from utente;
(\q per uscire)

4.2.2 Vedere i file di Viri
Viri=> select * from file;


5) KNOWN BUGS
=============

COG2085, heuristic "gdt universal", baco: sulla sinistra in basso, i genitori del 4°, 5°, 6° e 7° parassita sono sovrapposti (c'è un genitore del 4° e 7° sovrapposto con il genitore del 5° e 6°).

