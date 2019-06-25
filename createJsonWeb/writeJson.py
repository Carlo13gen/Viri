import json
from graph_analysis import analysis as ga

def trasformaGrafoInStringaJson(graph):
    #trasformo in json solo il primo grafo restituito come soluzione
    json_graph = []
    json_graph.append(ga.convert_graph_to_json(graph))
    return str(json.dumps(json_graph))