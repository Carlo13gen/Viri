

def verifica_file_compatibili(input, file):

    _input = input.stream
    _file = file.stream
    parentesi_host_nex = ""
    parentesi_parasite_nex = ""
    parentesi_host_out = ""
    parentesi_parasite_out = ""

    for line in _input:
        if line.find("TREE * Host") != -1 or line.find("TREE HOST" != -1):
            line_partition = line.partition("=")
            host_tree = line_partition[2]
            for c in host_tree:
                if c == "(" or c == ")":
                    parentesi_host_nex += c
        if line.find("TREE * Parasite") != -1 or line.find("TREE PARASITE" != -1):
            line_partition = line.partition("=")
            parasite_tree = line_partition[2]
            for c in parasite_tree:
                if c == "(" or c == ")":
                    parentesi_parasite_nex += c
        for line in _file:
            if line.find("#Host tree          ") != -1:
                line_partition = line.partition("=")
                host_tree_out = line_partition[2]
                for c in host_tree_out:
                    if c == "(" or c == ")":
                        parentesi_host_out += c
            if line.find("#Parasite tree      ") != -1:
                line_partition = line.partition("=")
                parasite_tree_out = line_partition[2]
                for c in parasite_tree_out:
                    if c == "(" or c == ")":
                        parentesi_parasite_out += c

    if (parentesi_host_nex == parentesi_host_out) and (parentesi_parasite_nex == parentesi_parasite_out):
        return True
    else:
        return False

