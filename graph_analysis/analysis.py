import networkx as nx
from networkx.readwrite import json_graph


def flag_switch_edges(parasite_tree, host_tree, mappings):

	new_parasite_tree = parasite_tree.copy()
	for edge in new_parasite_tree.edges():
		origin = edge[0]
		destination = edge[1]

		origin_mapping = mappings[origin]
		destination_mapping = mappings[destination]
		new_parasite_tree[origin][destination]["switch"] = True if not (nx.has_path(host_tree, origin_mapping,
																					destination_mapping)) else False
		new_parasite_tree[origin][destination]["from"] = edge[0];
		new_parasite_tree[origin][destination]["to"] = edge[1];
	return new_parasite_tree


def join_trees(host_tree, parasite_tree, mapping):

	# Copy the host tree
	graph = host_tree.copy()

	# Flag host edges as non-dummy, non-switch, non-roots-connector.
	# Also, assign "from" and "to" properties. This is useful because when converting the graph_analysis
	# into json through the function json_graph.node_link_data, networkx removes the names and uses the array
	# indexes instead. It does that because D3 does that :/
	for e in graph.edges(data=True):
		e[2]["dummy"] = False
		e[2]["switch"] = False
		e[2]["roots_connector"] = False
		e[2]["from"] = e[0]
		e[2]["to"] = e[1]

	# Add nodes of the parasite tree, with its properties
	for n in parasite_tree.nodes(data=True):
		graph.add_node(n[0], n[1])

	# Add edges of the parasite tree
	for e in parasite_tree.edges(data=True):
		e[2]["dummy"] = False
		e[2]["roots_connector"] = False
		e[2]["from"] = e[0]
		e[2]["to"] = e[1]
		graph.add_edge(e[0], e[1], e[2])

	# Get leaves of parasite tree
	parasite_tree_leaves = [n for n, d in parasite_tree.out_degree().items() if d == 0]
	
	# Additional edges between the leaves of host and the leaves of parasite
	for l in parasite_tree_leaves:
		graph.add_edge(mapping[l], l,
					   {"dummy": True,
						"switch": False,
						"roots_connector": False,
						"from": mapping[l],
						"to":l
						})

	# Additional edge between the roots
	host_root = [n for n, d in host_tree.in_degree().items() if d == 0][0]
	parasite_root = [n for n, d in parasite_tree.in_degree().items() if d == 0][0]
	graph.add_edge(host_root, parasite_root,
				   {"dummy": True,
					"switch": False,
					"roots_connector": True,
					"from": host_root,
					"to": parasite_root
					})
	return graph


# This function takes as input an embedding and the graph_analysis enclosing both host and parasite trees.
# It labels the nodes belonging to the host tree as right/left children of their parent based
# on the provided embedding
def label_host_tree_nodes(embedding, graph):

	roots_connector = [e for e in graph.edges(data=True) if e[2]["roots_connector"] == True][0]
	host_tree_root = roots_connector[0] if roots_connector[0].startswith("H") else roots_connector[1]

	#def labelLeafParassite(current_node, current_edge, embedding_current_node):
		#print current_node
        #print graph.neighbors(current_node)
        #embedding_current_node = embedding[current_node]
        #print enumerate(embedding_current_node)

	# This recursive function is a closure and accesses embedding and graph_analysis
	def labelLeafParassite(current_node, current_edge,embedding):
		embedding_current_node = embedding[current_node]

		#supporto.append(embedding_current_node[current_edge_position]);
		other_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if (edge[0].startswith("H") and edge[1].startswith("P")) or (edge[0] == current_edge[0] and edge[1] == current_edge[1]) ]
		if len(other_edge_position) == 1 :
			return

		supporto=[]
		other_edge_position = sorted( other_edge_position, cmp=lambda x,y: x-y )
		
		numeroArchiParassiti = len(other_edge_position)
		for i in range(0, numeroArchiParassiti):
			supporto.append(embedding_current_node[other_edge_position[i]]);
		
		current_edge_position = [idx for idx, edge in enumerate(supporto)
				if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]

		#def comparazioneArchiParassiti(x,y,numeroArchiParassiti,current_edge_position) :
		#	position_of_x = [idx for idx, edge in enumerate(supporto)
		#		if edge[0] == x[0] and edge[1] == x[1]][0]
		#	position_of_y =[idx for idx, edge in enumerate(supporto)
		#		if edge[0] == y[0] and y[1] == current_edge[1]][0]
		#	return ((position_of_x + current_edge_position) % numeroArchiParassiti)-((position_of_y + current_edge_position) % numeroArchiParassiti)
		quantoManca = numeroArchiParassiti - current_edge_position

		supporto2=[]
		for i in range(0, numeroArchiParassiti):
			nuovoIndicatore = (i + quantoManca) % numeroArchiParassiti;
			edge=supporto[i]
			parassite_to_label = edge[0] if current_node != edge[0] else edge[1]
			#if parassite_to_label.startswith("P") :
			graph.node[parassite_to_label]["position"] = nuovoIndicatore;
			graph.node[parassite_to_label]["MaxPosition"] = numeroArchiParassiti;
		return

	
	def label_nodes(current_node, current_edge):

		embedding_current_node = embedding[current_node]

		current_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if edge[0] == current_edge[0] and edge[1] == current_edge[1]][0]
		other_edge_position = [idx for idx, edge in enumerate(embedding_current_node)
								 if edge[0].startswith("H") and edge[1].startswith("H") ]
		

		# Stop condition: the current node is a leaf and has therefore only 2 incident edges
		if not((len(other_edge_position) == 3) or (len(other_edge_position) == 2)):
			labelLeafParassite(current_node, current_edge,embedding)
			return

		# Body
		right_edge = embedding_current_node[(current_edge_position + 1) % 3]
		left_edge = embedding_current_node[(current_edge_position + 2) % 3]

		right_child_to_label = right_edge[0] if current_node != right_edge[0] else right_edge[1]
		left_child_to_label = left_edge[0] if current_node != left_edge[0] else left_edge[1]

		graph.node[right_child_to_label]["right_child_gdt"] = True
		graph.node[right_child_to_label]["left_child_gdt"] = False

		graph.node[left_child_to_label]["left_child_gdt"] = True
		graph.node[left_child_to_label]["right_child_gdt"] = False

		# Recursive calls
		label_nodes(right_child_to_label, right_edge)
		label_nodes(left_child_to_label, left_edge)

	# The root is neither right nor left of anybody
	graph.node[host_tree_root]["left_child_gdt"] = False
	graph.node[host_tree_root]["right_child_gdt"] = False
	label_nodes(host_tree_root, roots_connector)

	return graph


def add_membership_to_nodes_of_host_tree(parasite_tree, mapping):

	for parasite, host in mapping.items():
		parasite_tree.node[parasite]["belongs_to"] = host

	return parasite_tree


def convert_graph_to_json(graph):
	return json_graph.node_link_data(graph)

