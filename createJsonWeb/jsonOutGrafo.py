from eucaliptFolder import parser2 as parser
from eucaliptFolder import eucalipt as eu
from createJsonWeb import writeJson as wj
from planareFolder import planare as pln
from planareFolder import funzioniSupporto as fs
from graph_analysis import analysis as ga
from euristicaFolder import euristica as eur
from euristicaFolder2 import euristica as eur2
from euristicaFolder import funzioniDiSupportoEuristica as fde
import json
import time
import re
import os

def findFreePosition(startPath,endPath):
	contatore = 1
	while os.path.isfile(startPath+str(contatore)+endPath):
		contatore = contatore + 1
	return str(contatore)
	

# WEB- GENERA I JSON CHE POI VENGONO STAMPATI
def get_graphs_using_gdt_cost(name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, rimuoviCicli, metodoRimuviCicli, soluzioniRandomiche, maximumHostSwitchDistance):	
	#provo a determinare la precision
	#try:
		#con time.time() prendo il tempo espresso in secondi
		tempoStartEucalipt = time.time()
		nameEucaliptOutput = findFreePosition("./eucaliptFolder/eucalypt_outputs/",".out")
		file_name = "./eucaliptFolder/eucalypt_outputs/"+nameEucaliptOutput+".out"
		eu.generaFileEucalipt(file_name, name,cospeciationCost,duplicationCost,hostswitchCost,sortingCost,number, rootToRoot, soluzioniRandomiche, maximumHostSwitchDistance);
		if (rimuoviCicli=="true") :
			eu.rimuoviCicli(file_name, metodoRimuviCicli)
		timeEndEucalipt = time.time()
		tempoImpiegatoEucalipt = '"tempoEucalipt" : '+str(timeEndEucalipt - tempoStartEucalipt)+" , ";
		# Parse data
		parsed_file = parser.parse_file(file_name)

		# Get trees and solutions
		original_host_tree = parsed_file["host_tree"]
		original_parasite_tree = parsed_file["parasite_tree"]
		solutions = parsed_file["solutions"]
		graphs_json_result=""

		if not (os.path.isfile("./memoria_embedding/"+name+".txt")):	
			i=0;
			root_mapping, possible_solutions = solutions.items()[0]
			mapping = possible_solutions[0];
			tempoStartTestPlanare = time.time()
			graph_embedding = pln.planareGDT(original_parasite_tree, original_host_tree, mapping);
			graph = graph_embedding["graph"]
			embending = graph_embedding["embe"]
			tempoEndTestPlanare = time.time()
			tempoImpiegatoPerPlanarita = ' "tempoPlanarita" : '+str(tempoEndTestPlanare - tempoStartTestPlanare) + " , ";

			tempoStartEuristicaGDT = time.time()
			if fs.isPlanare(graph , embending):
				isPlanGraph = True
				tempoImpiegatoEuristicaGDT = ' "tempoEurusticaGDT" : '+str(0) + " , ";
			else :
				isPlanGraph = False 
				result = eur.euristicaGDT(original_parasite_tree, original_host_tree, mapping, graph);
				graph = result["graph_solution"]
				embending = result["embedding"]
				tempoEndEuristicaGDT = time.time()
				tempoImpiegatoEuristicaGDT = ' "tempoEurusticaGDT" : '+str(tempoEndEuristicaGDT - tempoStartEuristicaGDT) + " , ";

			e={ "isPlanGraph" : isPlanGraph , "embending" : embending , "tempoImpiegatoPerPlanarita" : tempoImpiegatoPerPlanarita , "tempoImpiegatoEuristicaGDT" : tempoImpiegatoEuristicaGDT }

			json_embending = json.dumps(e)
			f = open("./memoria_embedding/"+name+".txt", 'w')
			f.write(json_embending)
			f.close()
		
		f = open("./memoria_embedding/"+name+".txt", 'r')
		json_embending = f.read()
		f.close()
		#print json_embending
		e = json.loads(json_embending)
		embending = e["embending"]
		isPlanGraph = e["isPlanGraph"]
		tempoImpiegatoPerPlanarita = e["tempoImpiegatoPerPlanarita"] 
		tempoImpiegatoEuristicaGDT = e["tempoImpiegatoEuristicaGDT"] 

		#with open('strings.json') as json_data:
    	#embending = json.load(json_embending)
    	#	print(d)

		for root_mapping, possible_solutions in solutions.iteritems():
			for mapping in possible_solutions:

				current_parasite_tree = ga.flag_switch_edges(original_parasite_tree, original_host_tree, mapping)
				current_parasite_tree = ga.add_membership_to_nodes_of_host_tree(current_parasite_tree, mapping)
				graphSupp = fs.makeGraph(current_parasite_tree, original_host_tree, mapping)
				graph = graphSupp[0];
				# se e planare riprendiamo l'embedding di prima e labelliamo il grafo
				tempoStartEuristicaGDTForSolution = time.time()
				if isPlanGraph :
					graphs_with_labelled_host_trees = []
					graphs_with_labelled_host_trees.append(ga.label_host_tree_nodes(embending, graph))
					graph = graphs_with_labelled_host_trees[0];
					tempoImpiegatoEuristicaGDTForSolution = ' "tempoEurusticaGDTForSolution" : '+str(0) + " , ";  
				
				else:
					graphs_with_labelled_host_trees = []
					graphs_with_labelled_host_trees.append(fde.label_host_tree_nodes(embending, graph))
					graph = graphs_with_labelled_host_trees[0];
					result = eur2.euristicaGDT(current_parasite_tree, original_host_tree, mapping, graph);
					graph = result["graph_solution"]
					tempoEndEuristicaGDTForSolution = time.time()
					tempoImpiegatoEuristicaGDTForSolution = ' "tempoEurusticaGDTForSolution" : '+str(tempoEndEuristicaGDTForSolution - tempoStartEuristicaGDTForSolution) + " , ";  

				#	result = eur.euristicaGDT(original_parasite_tree, original_host_tree, mapping, graph);
				#	graph = result["graph_solution"]
				#	embedding = result["embedding"] 
				
				# Returning the planar embedding of the first tuple
				stringaTempi=""+tempoImpiegatoEucalipt + tempoImpiegatoPerPlanarita + tempoImpiegatoEuristicaGDT + tempoImpiegatoEuristicaGDTForSolution 
				json_graph = wj.trasformaGrafoInStringaJson(graph);
				graphs_json_result += (",[{ "+stringaTempi+ json_graph[2:] ) 
		
		result = '[{"graph":['+graphs_json_result[1:]+'],"number":'+str(len(str(solutions.items()).split("}"))-1)+'}]'
		eu.pulisciServer(file_name);
		
		if  ((len(str(solutions.items()).split("}"))-1) == 0):
			return '[{"graph":[],"number":1 , "problem": "number of solution"}]'
		return result

	# commentati da titto (insieme al try) per vedere l'eccezione
	#except Exception:
	#	eu.pulisciServer(file_name);
	#	return '[{"graph":[],"number":1 , "problem": "number of solution"}]'
