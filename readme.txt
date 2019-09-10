
INSTALLAZIONE:

installare pip
installare flask
installare networkx (attenzione alla versione)
installare postgresql

Per configurare postgresql e creare tutte le tabelle necessarie avviare lo script install_postgres.sh e successivamente per testare che la creazione delle
tabelle sia avvenuta correttamente lanciare lo script test_postgres.sh

Successivamente vanno modificate le configurazioni di accesso a postgres. Dall'utente in cui ci si trova digitare sul terminale: psql -d Viri
Dopodichè il comando: ALTER USER "nome_utente" with password "nuova_password";

A questo punto dovrebbe essere cambiata la password per "nome_utente" in "nuova_password", modificare /persistance/persistance_handler.py
settando ogni valore indicato con le seguenti stringhe: USER = "nome_utente"
                                                        PASSWORD = "nuova_password"
                                                        HOST = "127.0.0.1"
                                                        PORT = "5432"
                                                        DATABASE = "Viri"


LANCIO:

Cambiare la variabile di sistema $TERM se necessario in gnome-terminal (TERM=gnome-terminal)
esegui ./avvio.sh
(si aprono 4 finestrelle)

apri un browser e connettiti a http://localhost:5000

SHUTDOWN:

esegui ./shutdown.sh
(vengono killate le 4 finestrelle e relativi processi)

DEBUG:

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

KNOWN BUGS:

COG2085, heuristic "gdt universal", baco: sulla sinistra in basso, i genitori del 4°, 5°, 6° e 7° parassita sono sovrapposti (c'è un genitore del 4° e 7° sovrapposto con il genitore del 5° e 6°).

