import networkx as nx
import re


def add_nodes_to_tree(tree_repr, graph):

    separators = ["(",")",",", ";"]
    for i, c in enumerate(tree_repr):

        # # skip first empty character and separators
        if c is " " or c in separators:
            continue

        # start of a node
        if c is "H" or c is "P":

            # build node name
            node_name = c
            for j in range(i+1, len(tree_repr), 1):
                next_char = str(tree_repr[j])
                if next_char in separators:
                    break
                node_name += next_char
            graph.add_node(node_name)


def build_node_from_position(pos_of_char, tree_repr):
    node_name = tree_repr[pos_of_char]
    numbers = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
    for i in range(pos_of_char + 1, len(tree_repr), 1):
        if tree_repr[i] not in numbers:
            break
        node_name += tree_repr[i]
    return node_name


def add_edges_to_tree(tree_repr, tree):

    for node in tree.nodes():

        # find node position
        node_position = re.search("\\b" + node + "\\b", tree_repr).start()
        char_on_left = tree_repr[node_position - 1]
        if char_on_left == "(":
            parenthesis_counter = 1
            for i in range(node_position, len(tree_repr), 1):
                if parenthesis_counter == 0:
                    father = build_node_from_position(i, tree_repr)
                    tree.add_edge(father, node)
                    break
                if tree_repr[i] == ")":
                    parenthesis_counter -= 1
                if tree_repr[i] == "(":
                    parenthesis_counter += 1


        elif char_on_left == ")":

            # go to left
            for i in reversed(range(0, node_position, 1)):
                if tree_repr[i] == "H" or tree_repr[i] == "P":
                    son = build_node_from_position(i, tree_repr)
                    tree.add_edge(node, son)
                    break

            # check if we have to go to right
            node_name = build_node_from_position(node_position, tree_repr)
            position_on_right = node_position + len(node_name)
            if tree_repr[position_on_right] == ",":
                parenthesis_counter = 1
                for j in range(position_on_right + 1, len(tree_repr), 1):
                    if parenthesis_counter == 0:
                        father = build_node_from_position(j, tree_repr)
                        tree.add_edge(father, node)
                        break
                    if tree_repr[j] == "(":
                        parenthesis_counter += 1

                    if tree_repr[j] == ")":
                        parenthesis_counter -= 1

        # The comma case should not happen for binary trees

    return tree


def parse_file(_file_name):

    host_tree = nx.DiGraph()
    parasite_tree = nx.DiGraph()
    solutions = {}

    _file = open(_file_name, 'r')
    for line in _file:

        # Host tree
        if line.find("#Host tree          ") != -1:

            # Split the line in two
            line_partition = line.rpartition("=")
            host_tree_repr = line_partition[2]

            add_nodes_to_tree(host_tree_repr, host_tree)
            add_edges_to_tree(host_tree_repr, host_tree)


        # Parasite tree
        if line.find("#Parasite tree      ") != -1:

            # Split the line in two
            line_partition = line.rpartition("=")
            parasite_tree_repr = line_partition[2]

            add_nodes_to_tree(parasite_tree_repr, parasite_tree)
            add_edges_to_tree(parasite_tree_repr, parasite_tree)

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
                my_assignments[assignment_partitioned[0]] = assignment_partitioned[2].replace(";", "")
            solutions[assignments[0]].append(my_assignments)

    return {"host_tree": host_tree, "parasite_tree": parasite_tree, "solutions": solutions}
