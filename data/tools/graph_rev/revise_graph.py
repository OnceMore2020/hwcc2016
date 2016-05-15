
def parse_path(path_line, topo_map):
    nodes = []
    path_ids = path_line.split('|')
    for path_id in path_ids:
        nodes.append(topo_map[path_id])
    nodes_uni = []
    nodes_uni.append(int(nodes[0][0]))
    for node in nodes:
        nodes_uni.append(int(node[1]))
    nodes_uni = sorted(nodes_uni)
    return nodes_uni


def to_str(nodes):
    nodes_line = ''
    for node in nodes:
        nodes_line += str(node) + '|'
    nodes_line += '\n'
    return nodes_line


def main():
    print('parsing topo...')
    topo_map = {}
    with open('topo.csv', 'r') as topo:
        for line in topo.readlines():
            infos = line[:-1].split(',')
            topo_map[infos[0]] = (infos[1], infos[2])
    print('parsing path...')
    with open('path.csv', 'r') as dmd:
        first_line = dmd.readline()
        second_line = dmd.readline()
        nodes_one = parse_path(first_line[:-1], topo_map)
        nodes_two = parse_path(second_line[:-1], topo_map)
        nodes_one_uni = []
        nodes_two_uni = []
        nodes_share = []

        for node in nodes_one:
            if node not in nodes_two:
                nodes_one_uni.append(node)
            else:
                nodes_share.append(node)

        for node in nodes_two:
            if node not in nodes_one:
                nodes_two_uni.append(node)


        nodes_one = to_str(nodes_one_uni)
        nodes_two = to_str(nodes_two_uni)
        nodes_share = to_str(nodes_share)
        with open('nodes.csv', 'w') as node_file:
            node_file.write(nodes_one)
            node_file.write(nodes_two)
            node_file.write(nodes_share)





if __name__ == '__main__':
    main()
