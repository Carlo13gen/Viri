import re
import networkx as nx


def parse_file(_file_name):
	host_tree = nx.DiGraph()
	parasite_tree = nx.DiGraph()
	solutions = {}
#	controllo = True
	
	_file = open(_file_name, 'r')
	for line in _file:
		if line.find("#Host tree          ") != -1:
			line_partition = line.rpartition("=")
			host_tree_repr = line_partition[2]
			separators = ["(",")",",", ";","\n"]
			contatore=0
			for i, c in enumerate(host_tree_repr):
				# # skip first empty character and separators
				if (not(contatore==0)):
					contatore -= 1
					continue
				if c is " " or c in separators:
					continue
				node_name = c
	
				j=i+1
				nextchar = str(host_tree_repr[j])
				while (not(nextchar in separators)) :
					node_name += nextchar
					contatore +=1
					j +=1
					nextchar = str(host_tree_repr[j])
				host_tree.add_node( "H"+node_name )
		if line.find("#Parasite tree      ") != -1:
			line_partition = line.rpartition("=")
			parassite_tree_repr = line_partition[2]
			separators = ["(",")",",", ";","\n"]
			contatore=0
			for i, c in enumerate(parassite_tree_repr):
				# # skip first empty character and separators
				if (not(contatore==0)):
					contatore -= 1
					continue
				if c is " " or c in separators:
					continue
				node_name = c
	
				j=i+1
				nextchar = str(parassite_tree_repr[j])
				while (not(nextchar in separators)) :
					node_name += nextchar
					contatore +=1
					j +=1
					nextchar = str(parassite_tree_repr[j])
				parasite_tree.add_node("P"+node_name)
		#archi
		if line.find("#Host tree          ") != -1:
			line_partition = line.rpartition("=")
			host_tree_repr = line_partition[2]
			separators = ["(",")",",", ";","\n"]
			contatore=0
			for node2 in host_tree.nodes():
				node = str(node2)[1:]
				# find node position
				#node_position = re.search("\\b" + node + "\\b", host_tree_repr).start()
				node_position = host_tree_repr.find(node)
				
				position_on_right = node_position + len(node)
				char_on_left = host_tree_repr[ position_on_right ]
				while (not(((host_tree_repr[node_position - 1] in separators)) and ((char_on_left in separators)))) :
					node_position = host_tree_repr[position_on_right:].find(node) + position_on_right
					position_on_right = node_position + len(node)
					char_on_left = host_tree_repr[ position_on_right ]
					


				char_on_left = host_tree_repr[node_position - 1]
				if char_on_left == "(":
					parenthesis_counter = 1
					i=node_position
					while ((not(parenthesis_counter == 0)) and (not(i==len(host_tree_repr)))) :
						if host_tree_repr[i] == ")":
							parenthesis_counter -= 1
						if host_tree_repr[i] == "(":
							parenthesis_counter += 1
						i += 1
					
					father_name = host_tree_repr[i]
					j = i+1
					nextchar = str(host_tree_repr[j])
					while(not(nextchar in separators)) :
						father_name += nextchar
						j +=1
						nextchar = str(host_tree_repr[j])
					host_tree.add_edge(("H"+father_name) , ("H" + node))
				
				if char_on_left == ")":
					i=node_position - 2
					son = host_tree_repr[i]
					i -= 1
					separators2 = [",",")"]
					while ((not(i==0))and(not(str(host_tree_repr[i]) in separators2))):
						son = host_tree_repr[i] + son
						i -= 1
					host_tree.add_edge(("H"+node) , ("H" + son))
					
	
	
	
					position_on_right = node_position + len(node)
					char_on_left = host_tree_repr[ position_on_right ]
	
					if char_on_left == ",":
						
						parenthesis_counter = 1
						i=position_on_right
						while ((not(parenthesis_counter == 0)) and (not(i==len(host_tree_repr)))) :
							if host_tree_repr[i] == "(":
								parenthesis_counter += 1
							if host_tree_repr[i] == ")":
								parenthesis_counter -= 1
							i += 1
						
						father_name = host_tree_repr[i]
						j = i+1
						nextchar = str(host_tree_repr[j])
						while(not(nextchar in separators)) :
							father_name += nextchar
							j +=1
							nextchar = str(host_tree_repr[j])
						host_tree.add_edge(("H"+father_name) , ("H" + node))
	
		
		if line.find("#Parasite tree      ") != -1:
			line_partition = line.rpartition("=")
			parassite_tree_repr = line_partition[2]
			separators = ["(",")",",", ";","\n"]
			contatore=0
			for node2 in parasite_tree.nodes():
				node = str(node2)[1:]
				# find node position
				#node_position = re.search("\\b" + node + "\\b", host_tree_repr).start()
				node_position = parassite_tree_repr.find(node)

				position_on_right = node_position + len(node)
				char_on_left = parassite_tree_repr[ position_on_right ]
				while (not(((parassite_tree_repr[node_position - 1] in separators)) and ((char_on_left in separators)))) :
					node_position = parassite_tree_repr[position_on_right:].find(node) + position_on_right
					position_on_right = node_position + len(node)
					char_on_left = parassite_tree_repr[ position_on_right ]
				





				char_on_left = parassite_tree_repr[node_position - 1]
				if char_on_left == "(":
					parenthesis_counter = 1
					i=node_position
					while ((not(parenthesis_counter == 0)) and (not(i==len(parassite_tree_repr)))) :
						if parassite_tree_repr[i] == ")":
							parenthesis_counter -= 1
						if parassite_tree_repr[i] == "(":
							parenthesis_counter += 1
						i += 1
					
					father_name = parassite_tree_repr[i]
					j = i+1
					nextchar = str(parassite_tree_repr[j])
					while(not(nextchar in separators)) :
						father_name += nextchar
						j +=1
						nextchar = str(parassite_tree_repr[j])
					parasite_tree.add_edge(("P"+father_name) , ("P" + node))
				
				if char_on_left == ")":
					i=node_position - 2
					son = parassite_tree_repr[i]
					i -= 1
					separators2 = [",",")"]
					while ((not(i==0))and(not(str(parassite_tree_repr[i]) in separators2))):
						son = parassite_tree_repr[i] + son
						i -= 1
					parasite_tree.add_edge(("P"+node) , ("P" + son))
					
	
	
	
					position_on_right = node_position + len(node)
					char_on_left = parassite_tree_repr[ position_on_right ]
	
					if char_on_left == ",":
						
						parenthesis_counter = 1
						i=position_on_right + 1
						while ((not(parenthesis_counter == 0)) and (not(i==len(parassite_tree_repr)))) :
							if parassite_tree_repr[i] == "(":
								parenthesis_counter += 1
							if parassite_tree_repr[i] == ")":
								parenthesis_counter -= 1
							i += 1
						
						father_name = parassite_tree_repr[i]
						j = i+1
						nextchar = str(parassite_tree_repr[j])
						while(not(nextchar in separators)) :
							father_name += nextchar
							j +=1
							nextchar = str(parassite_tree_repr[j])
						parasite_tree.add_edge(("P"+father_name) , ("P" + node))
	
		# Initialize dictionry of solution
		if line[0] is not "#":
			assignments = [x.strip() for x in line.split(',')]
			assignments[0] = assignments[0].replace(";", "")
			if assignments[0] not in solutions:
				solutions[assignments[0]] = []
	
		# Add solutions
		if line[0] is not "#":
			assignments = [x.strip() for x in line.split(',')]
			assignments[0] = assignments[0].replace(";", "")
	
			my_assignments = {}
			for assignment in assignments:
				assignment_partitioned = assignment.rpartition("@")
	#			if not(parasite_tree.__contains__("P"+assignment_partitioned[0])):
	#				controllo = False
	
	#			if not(host_tree.__contains__("H"+assignment_partitioned[2])):
	#				controllo = False
	
				my_assignments[("P"+assignment_partitioned[0])] = ("H"+assignment_partitioned[2]).replace(";", "")
			solutions[assignments[0]].append(my_assignments)
	
	#for nodo in parasite_tree:
	#	print len (parasite_tree.neighbors(str(nodo))) , nodo , parasite_tree.neighbors(str(nodo))

	#print controllo
	#print parasite_tree.edges()
	#print host_tree.edges()
	return {"host_tree": host_tree, "parasite_tree": parasite_tree, "solutions": solutions}