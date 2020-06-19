from flask import Flask
from createJsonWeb import jsonOutGrafo as inizio

app = Flask(__name__)
print(__name__)

from datetime import timedelta
from flask import make_response, request, current_app
from functools import update_wrapper

app.config['FLASK_RUN_PORT']=9000

def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, str):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, str):
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


@app.route('/graphs_gdt/<name>/<cospeciationCost>/<duplicationCost>/<hostswitchCost>/<sortingCost>/<number>/<rootToRoot>/<rimuoviCicli>/<metodoRimuviCicli>/<soluzioniRandomiche>/<maximumHostSwitchDistance>')
@crossdomain(origin="*")
def get_graphs_using_gdt_cost(name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, rimuoviCicli, metodoRimuviCicli,soluzioniRandomiche, maximumHostSwitchDistance):
	return inizio.get_graphs_using_gdt_cost(name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, rimuoviCicli, metodoRimuviCicli,soluzioniRandomiche, maximumHostSwitchDistance)


#if __name__ == '__main__':
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9000)
#app.debug = True
#app.run(host=app.config.get("HOST", "localhost"),port=9000)
#app.run()
