from graph_analysis import analysis as ga
from graph_analysis import gdt_adapter as gdt
from createJsonWeb import writeJson as wj

# FUNZIONI DI APPOGGIO
def provoGDT(graphs_from_joined_trees):
    try:
        # Compute a list of pairs: (planar embedding, graph_analysis)
        planar_embedding_and_graphs = []
        for graph in graphs_from_joined_trees:
            nx_planar_embedding = gdt.get_nx_planar_embedding(graph)
            planar_embedding_and_graphs.append((nx_planar_embedding, graph))
        # Compute a list of graphs where each node of the host tree is labelled (right or left child of its parent)
        graphs_with_labelled_host_trees = []
        for embedding_and_graph in planar_embedding_and_graphs:
            graphs_with_labelled_host_trees.append(ga.label_host_tree_nodes(embedding_and_graph[0], embedding_and_graph[1]))
        return { "graph": graphs_with_labelled_host_trees[0], "embe" : embedding_and_graph[0]};
    except Exception:
        return { "graph": graphs_from_joined_trees[0], "embe" : 0};

def makeGraph(current_parasite_tree, original_host_tree, mapping):
    #join with two tree
    graphs_from_joined_trees = []
    # Join the two trees
    graphs_from_joined_trees.append(ga.join_trees(original_host_tree, current_parasite_tree, mapping))
    return graphs_from_joined_trees;

def isPlanare(graph, embedding):
    return (embedding!=0)
    try:
        len(wj.trasformaGrafoInStringaJson(graph).split("right_child_gdt")[1]) ;
        return True;
    except Exception:
        return False;