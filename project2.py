from flask import Flask, jsonify
from flask import render_template
from flask import request, redirect
from werkzeug.utils import secure_filename
from flask_bcrypt import Bcrypt
import flask
import jinja2
import time
import os
import json
import datetime
from varie_supporto import verificaFileCompatibili as file_compatibili
from varie_supporto import check_input as check
from persistence import persistence_handler as perhand
import flask_login

app = Flask(__name__)
app.secret_key = 'Viri'
bcrypt = Bcrypt(app)

login_manager = flask_login.LoginManager()
login_manager.init_app(app)

class User(flask_login.UserMixin):
	pass

app.config["INPUT_UPLOAD"] = "input_data"
app.config["OUTPUT_UPLOAD"] = "eucaliptFolder/eucalypt_outputs"
app.config["ALLOWED_FILE_EXTENSIONS"] = ["NEX", "OUT"]
app.config["MAX_FILESIZE"] = 0.5 * 1024 * 1024
#Configura l'applicazione per conservare i token nei cookies
app.config['JWT_TOKEN_LOCATION'] = ['cookies']
#Configura il path in cui mandare i cookies
app.config['JWT_ACCESS_COOKIE_PATH'] = '/'


def allowed_filesize(filesize):
    if int(filesize) <= app.config["MAX_FILESIZE"]:
        return True
    else:
        return False

def allowed_file(filename, filename1):
	if not "." in filename:
		return False
	ext = filename.rsplit(".", 1)[1]

	if ext.upper() in app.config["ALLOWED_FILE_EXTENSIONS"]:
		return True
	else:
		False

# Templates
@app.route('/')
def home():
	return render_template('index.html')

# Templates
@app.route('/upload')
def upload():
	return render_template('upload.html')

# Templates
@app.route('/upload-graph', methods=["GET", "POST"])
def upload_file():

	#Controlla che il metodo di richiesta sia POST
	if request.method == "POST":

		#Prendi i file e salvali in delle variabili
		if request.files:

			if "filesize" in request.cookies:
				if not allowed_filesize(request.cookies["filesize"]):
					#print("Max file size exceeded")
					redirect(request.url)

			if "filesize1" in request.cookies:
				if not allowed_filesize(request.cookies["filesize1"]):
					#print("Max file size exceeded")
					redirect(request.url)

			input = request.files["input"]
			output = request.files["output"]

			if input.filename == "":
				#print("No filename")
				return redirect(request.url)

			if output.filename == "":
				#print("No filename")
				return redirect(request.url)

			if allowed_file(input.filename, output.filename) and file_compatibili.verifica_file_compatibili(input, output):
				input.stream.seek(0, 0)
				output.stream.seek(0, 0)
				name_partition_nex = input.filename.partition(".")
				name_partition_out = output.filename.partition(".")
				new_nex_name = name_partition_nex[0] + "_" + str(datetime.datetime.now().day) + "_" + str(datetime.datetime.now().month) + "_" + str(datetime.datetime.now().hour) + "_" + str(datetime.datetime.now().minute) + "_" + str(datetime.datetime.now().second) + "." + name_partition_nex[2]
				new_out_name = name_partition_out[0] + "_" + str(datetime.datetime.now().day) + "_" + str(datetime.datetime.now().month) + "_" + str(datetime.datetime.now().hour) + "_" + str(datetime.datetime.now().minute) + "_" + str(datetime.datetime.now().second) + "." + name_partition_out[2]
				input_filename = secure_filename(new_nex_name)
				output_filename = secure_filename(new_out_name)
				input.save(os.path.join(app.config["INPUT_UPLOAD"], input_filename))
				output.save(os.path.join(app.config["OUTPUT_UPLOAD"], output_filename))
				#print(input)
				#print(output)
				return redirect(request.url)
			else:
				x = 'The two input files are not compatible, check them and upload again!'
				ritorno = json.dumps(x)
				return render_template('upload.html', ritorno=ritorno)
	else:
		return render_template('index.html')

@login_manager.user_loader
def user_loader(username):
	data = perhand.find_user(username)
	username_user = data[0]
	if username_user == None:
		return
	else:
		user = User()
		user.id = username_user
		return user

@login_manager.request_loader
def request_loader(request):
	username = request.form.get('username')
	data = perhand.find_user(username)
	if data == None:
		return
	else:
		user = User()
		user.id = data
		user.is_authenticated = bcrypt.check_password_hash(data, request.form['password'])
		return user

@app.route('/login_form')
def login_form():
	return render_template('login.html')

@app.route('/register_form')
def register_form():
	return render_template('register.html')

@app.route('/register', methods=["GET", "POST"])
def register():

	if request.method == "POST":
		first_name = str(request.form['first'])
		last_name = str(request.form['last'])
		username = str(request.form['username'])
		password = str(request.form['password'])

		pwd_crypt = bcrypt.generate_password_hash(password).decode("utf-8")

		print(type(first_name))

		bool = perhand.save_user(first_name, last_name, username, pwd_crypt)

		if bool == 1:
			return render_template('login.html')
		if bool == 2:
			x = 'The username is already used, try with another one'
			ritorno = json.dumps(x)
			return render_template('register.html', ritorno=ritorno)
		else:
			x = 'Something went wrong, try again!'
			ritorno = json.dumps(x)
			return render_template('register.html', ritorno=ritorno)
	else:
		return render_template('login.html')

@app.route('/prova', methods=['GET'])
@flask_login.login_required
def prova():
	return 'Logged in as: ' + flask_login.current_user.id

@app.route('/login', methods=["POST"])
def login():
	username = flask.request.form['username']
	data = perhand.search_user_pwd(username)

	if data == None:
		x = "Wrong username"
		risultato = json.dumps(x)
		return render_template('login.html', risultato=risultato)
	else:
		user_password = data[0]
		if bcrypt.check_password_hash(user_password, request.form['password']):
			user = User()
			user.id = username
			flask_login.login_user(user)
			user_logged = json.dumps(username)
			return render_template('index.html', user_logged=user_logged)
		else:
			x = "Wrong password"
			risultato = json.dumps(x)
			return render_template('login.html', risultato=risultato)

# Templates
@app.route('/test')
def test():
	return render_template('test.html')

if __name__ == '__main__':
	app.debug = True
	app.run(host=app.config.get("HOST", "localhost"),port=app.config.get("PORT", 5000))
	#app.run()
