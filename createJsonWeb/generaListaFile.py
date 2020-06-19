import json
import os

def get_list_of_file(path):
    list_file=[]
    lista_file = os.listdir(path)
    for file in lista_file:
        cut = len( file ) -4
        cut_nex = file[0:cut]
        list_file.append(cut_nex)
    #no longer working in python 3
    #list_file=list_file + ( map( lambda x : x[0:(len(x)-4)],(filter(lambda x: x[(len(x)-4):len(x)]==".nex" ,os.listdir(path))) ))
    return json.dumps(list_file)