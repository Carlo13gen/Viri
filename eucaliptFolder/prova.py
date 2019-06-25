_file_name = "./pippo2.out"
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