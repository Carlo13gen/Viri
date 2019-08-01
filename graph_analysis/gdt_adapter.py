from lxml import etree
from subprocess import call
from graph_analysis import error as err
from flask import request
import json
from varie_supporto import cookie_decoder as decoder
from flask_cookie_decode import CookieDecode

#GDT_HOME = "/home/diego/Scrivania/tesi/paper_e_programmi_prof/project/graph_analysis/gdt/bin/x86_64/"
#GDT_HOME = "./graph_analysis/gdt/bin/x86_64/"
GDT_HOME = "./graph_analysis/gdt/bin/"


def remove_edge_closure(fixed_graph):
    return fixed_graph.replace("</EDGE>", "")


def fix_symbols(string_graph):
    return string_graph\
        .replace("&gt;", ">")\
        .replace("&lt;", "<")


def fix_new_lines_and_tabs_for_graph(string_graph):

    #Tutti i valori sono stati cambiati in tipo byte per ritrnare al punto di prima riportare a stringhe semplicemente levando b e rimenttendo doppi apici
    return string_graph\
        .replace("<NODELIST>", "\n\t<NODELIST>")\
        .replace("</NODELIST>", "\n\t</NODELIST>")\
        .replace("<NODE>", "\n\t\t<NODE>")\
        .replace("</NODE>", "\n\t\t</NODE>")\
        .replace("<EDGE>", "\n\t\t\t<EDGE>")\
        .replace("</UNDISECTION>", "\n</UNDISECTION>")


def fix_new_lines_and_tabs_for_constraints(string_constraints):

    return string_constraints\
        .replace("<BEGIN_OPTIONS>", "\n<BEGIN_OPTIONS>")\
        .replace("</BEGIN_OPTIONS>", "\n<END_OPTIONS>")\
        .replace("<CHECKING>", "\n\t<CHECKING>")\
        .replace("<ALGORITHM>", "\n\t<ALGORITHM>")\
        .replace("<CONSTRAINTS>", "\n\t<CONSTRAINTS>")\
        .replace("</CONSTRAINTS>", "\n\t</CONSTRAINTS>")\
        .replace("<UNCROSSABLE_EDGES>", "\n\t\t<UNCROSSABLE_EDGES>")\
        .replace("</UNCROSSABLE_EDGES>", "\n\t\t</UNCROSSABLE_EDGES>")\
        .replace("<EDGE>", "\n\t\t\t<EDGE>")\



def add_constraints_header(string_constraints):
    return '<BOFF VERSION="1.2"/>' + string_constraints.decode('utf-8')


def assign_ids_to_edges(graph):
    ids = 1
    for e in graph.edges(data=True):
        e[2]["id"] = ids
        ids += 1
    return graph


def assign_ids_to_nodes(graph):
    ids = 1
    for n in graph.nodes(data=True):
        n[1]["id"] = ids
        ids += 1
    return graph


def convert_graph_to_gdt(graph):

    root = etree.Element('UNDISECTION')
    node_list = etree.SubElement(root, 'NODELIST')

    assign_ids_to_edges(graph)
    assign_ids_to_nodes(graph)

    for node in graph.nodes(data=True):
        node_name = node[0]
        node_id = node[1]["id"]

        xml_node = etree.SubElement(node_list, 'NODE')

        # Add node
        xml_node.text = " " + str(node_id)

        # Incident edges
        out_edges = graph.out_edges(node_name, data=True)
        for out_e in out_edges:
            xml_edge = etree.SubElement(xml_node, 'EDGE')
            xml_edge.text = " " + str(out_e[2]["id"]) + " -> "
            #xml_edge.text = " " + str(out_e[2]["id"]) + " -- "

        in_edges = graph.in_edges(node_name, data=True)
        for in_e in in_edges:
            xml_edge = etree.SubElement(xml_node, 'EDGE')
            xml_edge.text = " " + str(in_e[2]["id"]) + " <- "
            #xml_edge.text = " " + str(in_e[2]["id"]) + " -- "

    # Get the graph_analysis
    string_graph = etree.tostring(root)
    final_graph = fix_symbols(string_graph.decode('utf-8'))
    final_graph = remove_edge_closure(final_graph)
    final_graph = fix_new_lines_and_tabs_for_graph(final_graph)

    return final_graph


def create_constraints_in_gdt(graph):
    # Uncrossable edges:
        # edges of the host tree
        # dummy edges between leaves of host and parasite
        # dummy edge between roots of host and parasite
        #   the last two points can be expressed simply considering all the "dummy" edges

    root = etree.Element("BEGIN_OPTIONS")

    # Checking
    checking = etree.SubElement(root, "CHECKING")
    checking.text = "   l     "

    # Algorith
    algorithm = etree.SubElement(root, "ALGORITHM")
    algorithm.text = "  11     "

    # Constraints
    constraints = etree.SubElement(root, "CONSTRAINTS")
    uncrossable_edges = etree.SubElement(constraints, "UNCROSSABLE_EDGES")

    for edge in graph.edges(data=True):
        # edges of host tree are incident to nodes whose name start with "H".
        if (edge[0].startswith("H") and edge[1].startswith("H")) or edge[2]["dummy"] == True:
            constraint = etree.SubElement(uncrossable_edges, "EDGE")
            constraint.text = str(" " + str(edge[2]["id"]))

    constraints = etree.tostring(root)
    constraints = add_constraints_header(constraints)
    constraints = remove_edge_closure(constraints)
    constraints = fix_new_lines_and_tabs_for_constraints(constraints)

    return constraints


def write_gdt_input_files_on_disk(gdt, constraints):

    #Da provare a decodificare il terzo elemento dello split
    cookie = request.cookies['session']
    cookie_str = decoder.decode(cookie)
    try :
        print(cookie_str)
        json_cookie = decoder.flask_loads(cookie_str)
        pretty = json.dumps(json_cookie,
                        sort_keys=True,
                        indent=4,
                        separators=(',', ': '))
    except Exception as e:
        return "[ERR: Not JSON data]"

    graph_file = open(GDT_HOME + "graph_file" + cookie, "w")
    graph_file.write(gdt)

    constraints_file = open(GDT_HOME + "blag" + cookie + ".ini", "w")
    constraints_file.write(constraints)


def run_gdt():
    cookie = request.cookies['session']
    cookie_str = decoder.decode(cookie)
    try:
        json_cookie = decoder.flask_loads(cookie_str)
        pretty = json.dumps(json_cookie,
                            sort_keys=True,
                            indent=4,
                            separators=(',', ': '))
    except Exception as e:
        return "[ERR: Not JSON data]"
    call(["./blag", "graph_file" + cookie, "blag" + cookie + ".ini"], cwd=GDT_HOME)


def parse_gdt_embedding(gdt_embedding_file):

    embedding = {}
    lines = gdt_embedding_file.readlines()
    for i in range(0, len(lines)):
        if lines[i].find("<NODE>") != -1:
            embedding[lines[i+1].strip()] = []
            j = i + 2
            while lines[j].find("<EDGE>") != -1:
                edge_id = [c for c in lines[j].split() if c.isdigit()][0]
                embedding[lines[i + 1].strip()].append(edge_id)
                j += 1

    return embedding


def convert_embedding_from_gdt_to_nx(gdt_embedding, nx_graph):

    def find_node_by_gdt_id(gdt_id, nx_graph):
        for node in nx_graph.nodes(data=True):
            if str(node[1]["id"]) == gdt_id:
                return node[0]
        raise err.NodeNotFound("gdt generated a node that is not in the graph_analysis.")

    def find_edge_by_gdt_id(gdt_id, nx_graph):
        for edge in nx_graph.edges(data=True):
            if str(edge[2]["id"]) == gdt_id:
                return (edge[0], edge[1])
        raise err.EdgeNotFound("gdt generated an edge that is not in the graph_analysis.")

    nx_embedding = {}

    for key, values in gdt_embedding.items():
        key_node = find_node_by_gdt_id(key, nx_graph)
        nx_embedding[key_node] = []
        for v in values:
            nx_embedding[key_node].append(find_edge_by_gdt_id(v, nx_graph))

    return nx_embedding


def planar_graph(gdt_embedding, nx_graph):
    # If the number of nodes of the embedding provided by gdt is the same as the number of nodes of the
    # original graph_analysis, it means that the graph_analysis was planar and no additional nodes were added.
    # If those two numbers are different, it means that gdt added some nodes to represent crossings.
    return len(gdt_embedding.keys()) == len(nx_graph.nodes())


# By nx planar embedding, we mean a dictionary where keys are the actual names of nodes and values are lists of actual
# names of nodes. GDT in fact uses numbers to represent nodes.
def get_nx_planar_embedding(graph):

    gdt_graph = convert_graph_to_gdt(graph)
    constraints = create_constraints_in_gdt(graph)

    write_gdt_input_files_on_disk(gdt_graph, constraints)
    run_gdt()  # This creates the file graph_file.gdt

    gdt_report = open(GDT_HOME + "gdt_report", "r")
    lines = gdt_report.readlines()
    if len(lines) != 11 :
        raise err.NonPlanarGraph("The input graph_analysis was bad")

    # From here: assume graph_file.gdt contains a planar embedding of the graph_analysis
    embedding_file = open(GDT_HOME + "graph_file.gdt", "r")
    gdt_embedding = parse_gdt_embedding(embedding_file)

    if planar_graph(gdt_embedding, graph):
        nx_embedding = convert_embedding_from_gdt_to_nx(gdt_embedding, graph)

        # To use gdt, we added integer ids to nodes and edges.
        # These ids are still in the graph_analysis, and since the instance is shared, before returning we remove those ids
        remove_gdt_ids_from_graph(graph)
        return nx_embedding

    else:
        raise err.NonPlanarGraph("The input graph_analysis was not planar!")


def remove_gdt_ids_from_graph(graph):

    for e in graph.edges(data=True):
        del e[2]["id"]

    for n in graph.nodes(data=True):
        del n[1]["id"]

    return graph
