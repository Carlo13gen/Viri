from flask import Flask
from flask import render_template
from flask import request
from createJsonWeb import jsonOutGrafo as inizio
from createJsonWeb import generaListaFile as glf
from createJsonWeb import nomeFile as nf
from salvaDati import salva

app = Flask(__name__)

from datetime import timedelta
from flask import make_response, request, current_app
from functools import update_wrapper


def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, basestring):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, basestring):
        origin = ', '.join(origin)
    if isinstance(max_age, timedelta):
        max_age = max_age.total_seconds()

    def get_methods():
        if methods is not None:
            return methods

        options_resp = current_app.make_default_options_response()
        return options_resp.headers['allow']

    def decorator(f):
        def wrapped_function(*args, **kwargs):
            if automatic_options and request.method == 'OPTIONS':
                resp = current_app.make_default_options_response()
            else:
                resp = make_response(f(*args, **kwargs))
            if not attach_to_all and request.method != 'OPTIONS':
                return resp

            h = resp.headers

            h['Access-Control-Allow-Origin'] = origin
            h['Access-Control-Allow-Methods'] = get_methods()
            h['Access-Control-Max-Age'] = str(max_age)
            if headers is not None:
                h['Access-Control-Allow-Headers'] = headers
            return resp

        f.provide_automatic_options = False
        return update_wrapper(wrapped_function, f)
    return decorator


@app.route('/salva/<nome>/<var>/<testo>')
@crossdomain(origin="*")
def metodoSalva(nome,var,testo):
	nomeFile = salva.salvaFile(nome,testo);
	return nf.returnNameFile(nomeFile)

@app.route('/inizializzaFile/<nome>/<var>/<testo>')
@crossdomain(origin="*")
def metodoInizializza(nome,var,testo):
	nomeFile = salva.inizializzaFile(nome,testo)
	# il var permette che il browser non usi la cache
	return nf.returnNameFile(nomeFile)

if __name__ == '__main__':
	app.debug = True
	app.run(host=app.config.get("HOST", "localhost"),port=app.config.get("PORT", 8888))
	#app.run()
