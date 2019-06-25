import os
import shutil

folder = "./data/json_outputs"
if (os.path.isdir(folder)):
	shutil.rmtree(folder)
	os.mkdir(folder)

folder = "./data/eucalypt_outputs"
if (os.path.isdir(folder)):
	shutil.rmtree(folder)
	os.mkdir(folder)