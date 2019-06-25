import json
import os

def get_list_of_file(path):
    list_file=[]
    list_file=list_file + ( map( lambda x : x[0:(len(x)-4)],(filter(lambda x: x[(len(x)-4):len(x)]==".nex" ,os.listdir(path))) ))
    return json.dumps(list_file)