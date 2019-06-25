import networkx as nx
from graph_analysis import analysis as ga
from planareFolder import funzioniSupporto as fs

def planareGDT(original_parasite_tree, original_host_tree, mapping):
    # etichetta host-switch
    current_parasite_tree = ga.flag_switch_edges(original_parasite_tree, original_host_tree, mapping)
    # Add to each node of the parasite tree, the host node it belongs to
    current_parasite_tree = ga.add_membership_to_nodes_of_host_tree(current_parasite_tree, mapping)
    graphs_from_joined_trees = fs.makeGraph(current_parasite_tree, original_host_tree, mapping)
    graph = fs.provoGDT(graphs_from_joined_trees)
    return graph;