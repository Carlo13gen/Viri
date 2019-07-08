def verifica_file_compatibili(input, output):
    _input = open(input, 'r')
    for line in _input:
        if line.find("	TREE * Host") != -1:
            line_partition = line.partition("=")
            host_tree = line_partition[2]
            parentesi_host_nex = ""
            for c in host_tree:
                if c == "(" or c == ")":
                    parentesi_host_nex += c

        if line.find("	TREE * Parasite") != -1:
            line_partition = line.partition("=")
            parasite_tree = line_partition[2]
            parentesi_parasite_nex = ""
            for c in parasite_tree:
                if c == "(" or c == ")":
                    parentesi_parasite_nex += c

    _output = open(output, 'r')
    for line in _output:
        if line.find("#Host tree          ") != -1:
            line_partition = line.partition("=")
            host_tree_out = line_partition[2]
            parentesi_host_out = ""
            for c in host_tree_out:
                if c == "(" or c == ")":
                    parentesi_host_out += c
        if line.find("#Parasite tree      ") != -1:
            line_partition = line_partition("=")
            parasite_tree_out = line_partition[2]
            parentesi_parasite_out = ""
            for c in parasite_tree_out:
                if c == "(" or c == ")":
                    parentesi_parasite_out += c

    if (parentesi_host_nex == parentesi_host_out) and (parentesi_parasite_nex == parentesi_parasite_out):
        return True
    else:
        False