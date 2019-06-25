from flask import Flask
from flask import render_template
from flask import request
from createJsonWeb import jsonOutGrafo as inizio
from createJsonWeb import generaListaFile as glf
from createJsonWeb import nomeFile as nf
from salvaDati import salva

app = Flask(__name__)

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
