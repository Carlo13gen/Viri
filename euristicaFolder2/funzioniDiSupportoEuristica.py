import networkx as nx
import functools

def makeGraph(host_tree,alberoRimasto,mapping,rootsOfGraph,host_root,parassite_root):
	# Copy the host tree
	graph = host_tree.copy()
	for e in graph.edges(data=True):
		e[2]["dummy"] = False
		e[2]["switch"] = False
		e[2]["roots_connector"] = False
		e[2]["from"] = e[0]
		e[2]["to"] = e[1]
	# Add nodes of the parasite tree, with its properties
	for n in alberoRimasto.nodes(data=True):
		graph.add_node(n[0], n[1])
	# Add edges of the parasite tree
	for e in alberoRimasto.edges(data=True):
		e[2]["dummy"] = False
		e[2]["switch"] = False
		e[2]["roots_connector"] = False
		e[2]["from"] = e[0]
		e[2]["to"] = e[1]
		graph.add_edge(e[0], e[1], e[2])
	# Get leaves of parasite tree
	alberoRimasto_leaves = [n for n, d in alberoRimasto.out_degree().items() if d == 0]
	# Additional edges between the leaves of host and the leaves of parasite
	for l in alberoRimasto_leaves:
		graph.add_edge(mapping[l], l,
					   {"dummy": False,
						"switch": False,
						"roots_connector": False,
						"from": mapping[l],
						"to":l
						})
	# Add edge Root
	#for l in rootsOfGraph:
	#	graph.add_edge(host_root, l,
	#				   {"dummy": False,
	#					"switch": False,
	#					"roots_connector": False,
	#					"from": host_root,
	#					"to":l
	#					})
	
	graph.add_edge(host_root, parassite_root,
				   {"dummy": True,
					"switch": False,
					"roots_connector": True,
					"from": host_root,
					"to":parassite_root
					})
	return graph

def creaListaArchiToltiEListaRoot(parassite_tree_root, graph):
	alberoRimasto = nx.DiGraph()
	rootsOfGraph = []
	alberoRimasto.add_node(parassite_tree_root);
	#rootsOfGraph.append(parassite_tree_root)
	archiTolti = []
	archiSwitch = []
	listaArchi = graph.edges();
	def visitaAlbero(graph,nodo,depth):
		edges = list(filter(lambda d: d[0] == nodo , listaArchi));
		if not (len(edges) == 0):
			edge1=edges[0]
			edge2=edges[1]
			
			result = qualeArcoRimuovere(edge1,edge2,nodo,graph);
			arcoRimasto = result["nodoRimasto"]
			arcoRimosso = result["nodoRimosso"]
			isSwitch = result["isSwitch"]

			rootsOfGraph.append(arcoRimosso[1])
			alberoRimasto.add_node(arcoRimosso[1]);
			alberoRimasto.add_node(arcoRimasto[1]);
			alberoRimasto.add_edge(arcoRimasto[0],arcoRimasto[1]);
			arco = []
			arco.append(arcoRimosso[0]);
			arco.append(arcoRimosso[1]);
			arco.append(depth);
			#archiTolti.append(arco);

			#archiTolti.append([].append(arcoRimosso[0],arcoRimosso[1]))

			result1= visitaAlbero(graph,arcoRimosso[1],(depth+1));
			result2= visitaAlbero(graph,arcoRimasto[1],(depth+1));
			if result1>result2:
				arco.append(result1);
				if isSwitch :
					archiSwitch.append(arco);
				else :
					archiTolti.append(arco);
				return (result1);
			else:
				arco.append(result2);
				if isSwitch :
					archiSwitch.append(arco);
				else :
					archiTolti.append(arco);
				return (result2);
		return depth;

	def cmpArchi(x,y):
		if (y[2] == x[2]) : 
			return x[3]-y[3]
		else :
			return y[2]-x[2]
	#def cmpArchi(x,y):
	#	if (y[3] == x[3]) : 
	#		return x[2]-y[2]
	#	else :
	#		return y[3]-x[3]


	visitaAlbero(graph,parassite_tree_root,0);

	archiTolti = sorted(archiTolti,  key=functools.cmp_to_key(lambda x,y: cmpArchi(x,y)))
	archiSwitch = sorted(archiSwitch, key=functools.cmp_to_key(lambda x,y: cmpArchi(x,y)))
	for arcoSwitch in archiSwitch : 
		archiTolti.append(arcoSwitch);
	#archiTolti.concat(archiSwitch) 
	#

	#print archiTolti
	# archiTolti = sorted(archiTolti, cmp=lambda x,y: y[2]-x[2] )
	# archiTolti = sorted(archiTolti, cmp=lambda x,y: x[2]-y[2] )
	
	#def togliDepth(x):
	#	arco=[]
	#	arco.append(x[0])
	#	arco.append(x[1])
	#	return arco;

	#archiTolti = map( togliDepth , archiTolti);

	#print rootsOfGraph;
	#print archiTolti;
	return { "alberoRimasto" : alberoRimasto , "archiTolti" : archiTolti , "rootsDelGrafo" : rootsOfGraph }
	
def qualeArcoRimuovere(arco1, arco2, nodo, graph):
	#print graph.nodes()
	#print arco1
	#print arco2
	if (graph.get_edge_data(arco1[0],arco1[1])["switch"]) : 
		return { "nodoRimosso" : arco1 , "nodoRimasto" : arco2 ,"isSwitch" : True }
	if (graph.get_edge_data(arco2[0],arco2[1])["switch"]) : 
		return { "nodoRimosso" : arco2 , "nodoRimasto" : arco1 ,"isSwitch" : True }
	return { "nodoRimosso" : arco1 , "nodoRimasto" : arco2 ,"isSwitch" : False }


def label_host_tree_nodes(embedding, graph):

	roots_connector = [e for e in graph.edges(data=True) if e[2]["roots_connector"] == True][0]
	host_tree_root = roots_connector[0] if roots_connector[0].startswith("H") else roots_connector[1]

	#def labelLeafParassite(current_node, current_edge, embedding_current_node):
		#print current_node
		#print graph.neighbors(current_node)
		#embedding_current_node = embedding[current_node]
		#print enumerate(embedding_current_node
	# This recursive function is a closure and accesses embedding and graph_analysis
	def labelLeafParassite(current_node, current_edge,embedding):
		embedding_current_node = embedding[current_node]

		#supporto.append(embedding_current_node[current_edge_position]);
		other_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if (edge[0].startswith("H") and edge[1].startswith("P")) or (edge[0] == current_edge[0] and edge[1] == current_edge[1]) ]
		if len(other_edge_position) == 1 :
			return

		supporto=[]
		# key=functools.cmp_to_key(lambda x,y: x-y)
		other_edge_position = sorted( other_edge_position, key=functools.cmp_to_key(lambda x,y: x-y))
		
		numeroArchiParassiti = len(other_edge_position)
		
		def aggiornaSupporto(supporto,i,numeroArchiParassiti,embedding_current_node,other_edge_position):
			supporto.append(embedding_current_node[other_edge_position[i]])
			if not (i == numeroArchiParassiti-1) :
				i=i+1;
				return aggiornaSupporto(supporto,i,numeroArchiParassiti,embedding_current_node,other_edge_position)
			else :
				return supporto

		supporto = aggiornaSupporto(supporto,0,numeroArchiParassiti,embedding_current_node,other_edge_position)

		#for i in range(0, numeroArchiParassiti):
		#	supporto.append(embedding_current_node[other_edge_position[i]]);
		
		current_edge_position = [idx for idx, edge in enumerate(supporto)
				if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]

		#def comparazioneArchiParassiti(x,y,numeroArchiParassiti,current_edge_position) :
		#	position_of_x = [idx for idx, edge in enumerate(supporto)
		#		if edge[0] == x[0] and edge[1] == x[1]][0]
		#	position_of_y =[idx for idx, edge in enumerate(supporto)
		#		if edge[0] == y[0] and y[1] == current_edge[1]][0]
		#	return ((position_of_x + current_edge_position) % numeroArchiParassiti)-((position_of_y + current_edge_position) % numeroArchiParassiti)
		quantoManca = numeroArchiParassiti - current_edge_position

		

		def aggiungerePosizioneAParassita(i, numeroArchiParassiti, quantoManca, supporto, current_node, graph) :
			nuovoIndicatore = (i + quantoManca) % numeroArchiParassiti;
			edge=supporto[i]
			parassite_to_label = edge[0] if current_node != edge[0] else edge[1]
			#if parassite_to_label.startswith("P") :
			graph.node[parassite_to_label]["position_solution"] = nuovoIndicatore;
			graph.node[parassite_to_label]["MaxPosition"] = numeroArchiParassiti;
			if not (i == numeroArchiParassiti-1) :
				i=i+1;
				return aggiungerePosizioneAParassita(i, numeroArchiParassiti, quantoManca, supporto, current_node, graph)
			else :
				return 
		aggiungerePosizioneAParassita(0, numeroArchiParassiti, quantoManca, supporto, current_node, graph)	
		
		#for i in range(0, numeroArchiParassiti):
		#	nuovoIndicatore = (i + quantoManca) % numeroArchiParassiti;
		#	edge=supporto[i]
		#	parassite_to_label = edge[0] if current_node != edge[0] else edge[1]
		#	#if parassite_to_label.startswith("P") :
		#	graph.node[parassite_to_label]["position"] = nuovoIndicatore;
		#	graph.node[parassite_to_label]["MaxPosition"] = numeroArchiParassiti;
		#	#print parassite_to_label
		#	#print nuovoIndicatore
		
		return

	def label_nodes(current_node, current_edge):

		embedding_current_node = embedding[current_node]

		#supporto.append(embedding_current_node[current_edge_position]);
		other_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if edge[0].startswith("H") and edge[1].startswith("H") ]

		# Stop condition: the current node is a leaf and has therefore only 2 incident edges
		if len(other_edge_position) != 3:
			labelLeafParassite(current_node, current_edge,embedding)
			return

		supporto=[]
		other_edge_position = sorted( other_edge_position, key=functools.cmp_to_key(lambda x,y: x-y))
		
		supporto.append(embedding_current_node[other_edge_position[0]]);
		supporto.append(embedding_current_node[other_edge_position[1]]);
		supporto.append(embedding_current_node[other_edge_position[2]]);

		current_edge_position = [idx for idx, edge in enumerate(supporto)
						 if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]
		

		# Body
		right_edge = supporto[(current_edge_position + 1) % 3]
		left_edge = supporto[(current_edge_position + 2) % 3]

		right_child_to_label = right_edge[0] if current_node != right_edge[0] else right_edge[1]
		left_child_to_label = left_edge[0] if current_node != left_edge[0] else left_edge[1]

		graph.node[right_child_to_label]["right_child_euristica2_solution"] = True
		graph.node[right_child_to_label]["left_child_euristica2_solution"] = False

		graph.node[left_child_to_label]["left_child_euristica2_solution"] = True
		graph.node[left_child_to_label]["right_child_euristica2_solution"] = False

		# Recursive calls
		label_nodes(right_child_to_label, right_edge)
		label_nodes(left_child_to_label, left_edge)
	
	def label_nodes_root(current_node, current_edge):

		embedding_current_node = embedding[current_node]

		supporto=[]

		current_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]
		
		#supporto.append(embedding_current_node[current_edge_position]);
		other_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if (edge[0].startswith("H") and edge[1].startswith("H"))]
		
		other_edge_position.append(current_edge_position)
		other_edge_position = sorted( other_edge_position, key=functools.cmp_to_key(lambda x,y: x-y))

		supporto.append(embedding_current_node[other_edge_position[0]]);
		supporto.append(embedding_current_node[other_edge_position[1]]);
		supporto.append(embedding_current_node[other_edge_position[2]]);

		current_edge_position = [idx for idx, edge in enumerate(supporto)
						 if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]
		

		
		# Stop condition: the current node is a leaf and has therefore only 2 incident edges
		if len(supporto) != 3:
			#labelLeafParassite(current_node, current_edge,embedding_current_node)
			return

		# Body
		right_edge = supporto[(current_edge_position + 1) % 3]
		left_edge = supporto[(current_edge_position + 2) % 3]

		right_child_to_label = right_edge[0] if current_node != right_edge[0] else right_edge[1]
		left_child_to_label = left_edge[0] if current_node != left_edge[0] else left_edge[1]

		graph.node[right_child_to_label]["right_child_euristica2_solution"] = True
		graph.node[right_child_to_label]["left_child_euristica2_solution"] = False

		graph.node[left_child_to_label]["left_child_euristica2_solution"] = True
		graph.node[left_child_to_label]["right_child_euristica2_solution"] = False

		# Recursive calls
		label_nodes(right_child_to_label, right_edge)
		label_nodes(left_child_to_label, left_edge)

	# The root is neither right nor left of anybody
	graph.node[host_tree_root]["left_child_euristica2_solution"] = False
	graph.node[host_tree_root]["right_child_euristica2_solution"] = False
	label_nodes_root(host_tree_root, roots_connector)

	return graph