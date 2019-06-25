from flask import Flask
from flask import render_template
from flask import request
from createJsonWeb import jsonOutGrafo as inizio
from createJsonWeb import generaListaFile as glf
from createJsonWeb import nomeFile as nf
from salvaDati import salva

app = Flask(__name__)

@app.route('/list_of_file')
def get_list_of_file():
	return glf.get_list_of_file("./input_data")

@app.route('/list_of_output_test')
def get_list_of_output_test():
	return glf.get_list_of_file("./output_test")


@app.route('/graphs_gdt/<name>/<cospeciationCost>/<duplicationCost>/<hostswitchCost>/<sortingCost>/<number>/<rootToRoot>/<rimuoviCicli>/<metodoRimuviCicli>/<soluzioniRandomiche>/<maximumHostSwitchDistance>')
def get_graphs_using_gdt_cost(name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, rimuoviCicli, metodoRimuviCicli,soluzioniRandomiche, maximumHostSwitchDistance):
	return inizio.get_graphs_using_gdt_cost(name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, rimuoviCicli, metodoRimuviCicli,soluzioniRandomiche, maximumHostSwitchDistance)

@app.route('/salva/<nome>/<var>/<testo>')
def metodoSalva(nome,var,testo):
	nomeFile = salva.salvaFile(nome,testo);
	return nf.returnNameFile(nomeFile)

@app.route('/inizializzaFile/<nome>/<var>/<testo>')
def metodoInizializza(nome,var,testo):
	nomeFile = salva.inizializzaFile(nome,testo)
	# il var permette che il browser non usi la cache
	return nf.returnNameFile(nomeFile)

# Templates
@app.route('/')
def home():
	return render_template('index.html')

# Templates
@app.route('/test')
def test():
	return render_template('test.html')



if __name__ == '__main__':
	app.debug = True
	app.run(host=app.config.get("HOST", "localhost"),port=app.config.get("PORT", 5000))
	#app.run()
