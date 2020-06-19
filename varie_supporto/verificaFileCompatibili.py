def verifica_file_compatibili(input, file):

    _input = input.stream
    _file = file.stream
    parentesi_host_nex = ""
    parentesi_parasite_nex = ""
    parentesi_host_out = ""
    parentesi_parasite_out = ""

    for line in _input:
        if (line.find(b'TREE * Host')) != -1 or (line.find(b'TREE HOST')) != -1:
            line_partition = line.partition(b'=')
            print(line_partition)
            host_tree = str(line_partition[2])
            print(host_tree)
            for c in host_tree:
                if str(c) == "(" or str(c) == ")":
                    parentesi_host_nex += str(c)
        if line.find(b'TREE * Parasite') != -1 or line.find(b'TREE PARASITE') != -1:
            line_partition = line.partition(b'=')
            parasite_tree = str(line_partition[2])
            for c in parasite_tree:
                if str(c) == "(" or str(c) == ")":
                    parentesi_parasite_nex += str(c)
        for line in _file:
            if line.find(b'#Host tree          ') != -1:
                line_partition = line.partition(b'=')
                host_tree_out = str(line_partition[2])
                for c in host_tree_out:
                    if str(c) == "(" or str(c) == ")":
                        parentesi_host_out += c
            if line.find(b'#Parasite tree      ') != -1:
                line_partition = line.partition(b'=')
                parasite_tree_out = str(line_partition[2])
                for c in parasite_tree_out:
                    if str(c) == "(" or str(c) == ")":
                        parentesi_parasite_out += str(c)
    print(parentesi_host_nex)
    print(parentesi_host_out)
    print(parentesi_parasite_nex)
    print(parentesi_parasite_out)
    if (parentesi_host_nex == parentesi_host_out) and (parentesi_parasite_nex == parentesi_parasite_out):
        return True
    else:
        return False

