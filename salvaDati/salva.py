import os
import re

def salvaFile(nome,testo):
	out_file = open("./output_test/"+nome+".txt","a")
	testo = testo.replace(".",",")
	out_file.write( testo+"\n")
	out_file.close()
	return nome

def inizializzaFile(nome,testo):
	contatore = 0
	nomeFile = nome
	if os.path.isfile("./output_test/"+nomeFile+".txt"):
		contatore = 1
		while os.path.isfile("./output_test/"+nomeFile+".txt"):
			nomeFile = nome+str(contatore)
			contatore = contatore + 1
	out_file = open("./output_test/"+nomeFile+".txt","w")
	out_file.write( testo+"\n")
	out_file.close()
	return nomeFile