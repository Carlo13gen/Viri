from flask import Flask
from flask import render_template
from flask import request, redirect
from flask import request
from werkzeug.utils import secure_filename
import os
from createJsonWeb import jsonOutGrafo as inizio
from createJsonWeb import generaListaFile as glf
from createJsonWeb import nomeFile as nf
from salvaDati import salva

app = Flask(__name__)

app.config["INPUT_UPLOAD"] = "input_data"
app.config["OUTPUT_UPLOAD"] = "eucaliptFolder/eucalypt_outputs"
app.config["ALLOWED_FILE_EXTENSIONS"] = ["NEX", "OUT"]
app.config["MAX_FILESIZE"] = 0.5 * 1024 * 1024

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
					print("Max file size exceeded")
					redirect(request.url)

			if "filesize1" in request.cookies:
				if not allowed_filesize(request.cookies["filesize1"]):
					print("Max file size exceeded")
					redirect(request.url)

			input = request.files["input"]
			output = request.files["output"]

			if input.filename == "":
				print("No filename")
				return redirect(request.url)

			if output.filename == "":
				print("No filename")
				return redirect(request.url)


			if allowed_file(input.filename, output.filename):
				input_filename = secure_filename(input.filename)
				output_filename1 = secure_filename(output.filename)
				input.save(os.path.join(app.config["INPUT_UPLOAD"], input_filename))
				output.save(os.path.join(app.config["OUTPUT_UPLOAD"], output_filename1))
				print(input)
				print(output)
				return redirect(request.url)
			else:
				print("File extension not allowed")
				return redirect(request.url)

	return render_template('index.html')

# Templates
@app.route('/test')
def test():
	return render_template('test.html')

if __name__ == '__main__':
	app.debug = True
	app.run(host=app.config.get("HOST", "localhost"),port=app.config.get("PORT", 5000))
	#app.run()
