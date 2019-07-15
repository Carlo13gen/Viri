import networkx as nx
import math
from euristicaFolder2 import funzioniDiSupportoEuristica as fde
from graph_analysis import gdt_adapter as gdt
import os

def euristicaGDT(original_parasite_tree, original_host_tree, mapping, graph5):
	#print graph.nodes();
	roots_connector = [e for e in graph5.edges(data=True) if e[2]["roots_connector"] == True][0]
	parassite_tree_root = roots_connector[0] if roots_connector[0].startswith("P") else roots_connector[1]
	host_tree_root = roots_connector[0] if roots_connector[0].startswith("H") else roots_connector[1]
	#print rootsOfGraph
	risultato = fde.creaListaArchiToltiEListaRoot(parassite_tree_root, original_parasite_tree);
	alberoRimasto = risultato["alberoRimasto"]
	archiTolti = risultato["archiTolti"]
	rootsOfGraph = risultato["rootsDelGrafo"]
	graph2 = fde.makeGraph(original_host_tree,alberoRimasto,mapping,rootsOfGraph,host_tree_root,parassite_tree_root);
	result = provoGDT(graph2, graph5);
	graph4 = result["graph_solution"]
	old_embedding = result["embedding"]

	numeroDiArchiTolti = len(archiTolti);
	i=0
	if numeroDiArchiTolti < 9 :
		#non salto mai
		lunghezzaSalto = 2
	else:
		lunghezzaSalto = int(math.ceil(math.sqrt(numeroDiArchiTolti)))-1
	print(lunghezzaSalto)
	#print graph5.edges()

	while i<numeroDiArchiTolti :
		if ((((i) % (lunghezzaSalto)) == 0) and ((i+lunghezzaSalto) < numeroDiArchiTolti)) :
			result = provaSalto(lunghezzaSalto, i, graph4, graph2, graph5, archiTolti, old_embedding, host_tree_root)
			graph2 = result["grafo_creato"];
			graph4 = result["grafo_risultato"];
			old_embedding = result["embedding"]
			i = i+result["indiceNuovo"];
			#print i
		else:
			result = provaAggiungereArco(graph4,graph2, graph5,i,archiTolti,old_embedding, host_tree_root);
			graph2 = result["grafo_creato"];
			graph4 = result["grafo_risultato"];
			old_embedding = result["embedding"]
			i=i+1
	return { "graph_solution": graph4 , "embedding" : old_embedding };

def provaSalto(lunghezzaSalto, indice, old_graph_solution, graph_create, graph_original,listaArchi, old_embedding, host_tree_root):
	old_indice=indice;
	try:
		graph_create2 = graph_create.copy();
		finoADoveSaltare = indice + lunghezzaSalto;
		indiceArco= indice
		while (indiceArco < finoADoveSaltare) :
			graph_create2.add_edge(listaArchi[indiceArco][0], listaArchi[indiceArco][1],
					{"dummy": False,
					"switch": False,
					"roots_connector": False,
					"from": listaArchi[indiceArco][0],
					"to":listaArchi[indiceArco][1]
					})
			#if graph_create2.has_edge(host_tree_root, listaArchi[indiceArco][1]) :
			#graph_create2.remove_edge(host_tree_root, listaArchi[indiceArco][1])

			indiceArco = indiceArco + 1;

		#graph_solution= provoGDT(graph_create2, graph_original);
		result = provoGDT(graph_create2, graph_original);
		graph_solution = result["graph_solution"];
		new_embedding = result["embedding"]
		print("yuppppiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii")
		return { "grafo_creato" : graph_create2 , "grafo_risultato" : graph_solution , "indiceNuovo" : lunghezzaSalto , "embedding" : new_embedding }
	except Exception:
		result = provaAggiungereArco(old_graph_solution, graph_create, graph_original,indice,listaArchi, old_embedding, host_tree_root);
		graph_create2 = result["grafo_creato"];
		old_graph_solution = result["grafo_risultato"];
		new_embedding = result["embedding"]
		lunghezzaSalto = 1;
		return { "grafo_creato" : graph_create , "grafo_risultato" : old_graph_solution, "indiceNuovo" : lunghezzaSalto , "embedding" : new_embedding }

def provaAggiungereArco(old_graph_solution, graph_create, graph_original,indiceArco,listaArchi, old_embedding, host_tree_root):
	try:
		graph_create2= graph_create.copy();
		graph_create2.add_edge(listaArchi[indiceArco][0], listaArchi[indiceArco][1],
				{"dummy": False,
				"switch": False,
				"roots_connector": False,
				"from": listaArchi[indiceArco][0],
				"to":listaArchi[indiceArco][1]
				})
		#graph_create2.remove_edge(host_tree_root, listaArchi[indiceArco][1])
		result = provoGDT(graph_create2, graph_original);
		graph_solution = result["graph_solution"];
		new_embedding = result["embedding"]
		return { "grafo_creato" : graph_create2 , "grafo_risultato" : graph_solution , "embedding" : new_embedding }
	except Exception:
		#graph_create.remove_edge(host_tree_root, listaArchi[indiceArco][1])
		return { "grafo_creato" : graph_create , "grafo_risultato" : old_graph_solution , "embedding" : old_embedding }


def provoGDT(graph_create,graph_original):

	graphs_from_joined_trees=[]
	graphs_from_joined_trees.append(graph_create);

	# Compute a list of pairs: (planar embedding, graph_analysis)
	planar_embedding_and_graphs = []
	for graph3 in graphs_from_joined_trees:
		nx_planar_embedding = gdt.get_nx_planar_embedding(graph3)
		planar_embedding_and_graphs.append((nx_planar_embedding, graph3))
	# Compute a list of graphs where each node of the host tree is labelled (right or left child of its parent)
	graphs_with_labelled_host_trees = []
	for embedding_and_graph in planar_embedding_and_graphs:
		graphs_with_labelled_host_trees.append( fde.label_host_tree_nodes(embedding_and_graph[0], graph_original))
	graph4 = graphs_with_labelled_host_trees[0]
	
	return { "graph_solution": graph4 , "embedding" : embedding_and_graph[0] };








	# (label da mettere al nodo host d["right_child_euristica2"]; d["left_child_euristica2"]; per la root metterli entrambi a false)
	# label_host_tree_nodes(embedding, graph) in analisis in graph_analisis sara simile alla nostra se:
	#   metiamo solo un rootConnector e dobbiamo cambiare il tipo di label

	# etichetta host-switch
	#print original_host_tree.nodes()
	#current_parasite_tree = ga.flag_switch_edges(original_parasite_tree, original_host_tree, mapping)
	# Add to each node of the parasite tree, the host node it belongs to
	#current_parasite_tree = ga.add_membership_to_nodes_of_host_tree(current_parasite_tree, mapping)
	#graphs_from_joined_trees = fs.makeGraph(current_parasite_tree, original_host_tree, mapping)
	#graph = fs.provoGDT(graphs_from_joined_trees)
	#print fs.isPlanare(graph)


	# i tre passi successivi si devono ripetere ciclicamente aggiuggiendo ogni volta un arco
	# per ottimizzare i tempi considerare
	#	1- il grafo intero e planare
	#	2- il grafo senza switch e planare
	#	3- considerare salti di lista archi 
	#			lunghi int(math.ceil(math.sqrt(NumeroDiArchi)))
	#			ogni ((numeroArcoNellaListaArchi % int(math.ceil(math.sqrt(NumeroDiArchiTolti))))==0)
	# problema come si aggiungono e tolgono archi dall'albero dei parassiti prima del test?
	#Join with two tree
	#
	# per avere la lista di archi: print original_parasite_tree.edges()
	# per creare un nuovo grafo:
	#
	# Flag switch edges
	# etichetto gli host-switch
	# current_parasite_tree = ga.flag_switch_edges(original_parasite_tree, original_host_tree, mapping)
	# print current_parasite_tree.nodes()
	#
	# graphs_from_joined_trees = makeGraph(current_parasite_tree, original_host_tree, mapping)
	# provoGDT prova a planarizzare e restituice il grafo con host gia etichettati se e planare.
	# print graphs_from_joined_trees[0].nodes() 
	# print original_host_tree.nodes()
	#DA RIFARE MAKEGRAPH il quale deve restituire la lista di grafi con il grafo come vogliamo ovvero fatto con il modulo networkx
	#
	# graph = provoGDT(graphs_from_joined_trees)
	# isPlanare vede se host sono etichettati
	# print isPlanare(graph)
	#
	#PRENDERE E AGGIUNGERE O RIMUOVERE UN ARCO
	# print original_parasite_tree.edges()
	# rimuovere il primo arco della lista.
	# original_parasite_tree.remove_edge( original_parasite_tree.edges()[0][0],original_parasite_tree.edges()[0][1] )
	# restituisce true o false in base al valore dello switch
	# print graph.get_edge_data(original_parasite_tree.edges()[0][0],original_parasite_tree.edges()[0][1])["switch"]
	# copiare l'albero dei parassiti
	# nuovoGrafo=original_parasite_tree.copy();
	# alla copia aggiungere l'arco tolto e poi provare la planarizzazione se no restituisce la vecchia copia del grafo se si la nuova.
	#
	#COME TOGLIERE UNO DEI DUE ARCHI PARTENTI DA UN NODO
	# prendere tutti i nodi del grafo dei parassiti.
	# print original_parasite_tree.nodes() 
	# per ognuno di essi vedere i vicini(che in questo caso sono i nodi figli)
	# print original_parasite_tree.neighbors(original_parasite_tree.nodes()[0])
	# vedo se tra questi sue una di questi due e unito con un arco host switch
	# print current_parasite_tree.get_edge_data(original_parasite_tree.edges()[0][0],original_parasite_tree.edges()[0][1])["switch"]
	# se si tolgo l'host-switch oppure elimino un arco a caso
	#  RICORDA LABELLARE ARCHI E NODI PRIMA DI DARLO IN PASTO A GDT COME FATTO IN JOIN_TREE IN ANALISIS
	# return graph;
