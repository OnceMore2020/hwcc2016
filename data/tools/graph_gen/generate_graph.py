#!/usr/bin/env python
# encoding: utf-8

import sys
import random
import networkx as nx
import matplotlib.pyplot as plt


def help_func():
    print('Generate Graph topo file and corresponding demand file used in hwcc2016')
    print('\tUsage:')
    print('\t./geneate_graph.py topo.csv demand.csv reference.csv')


def check_out_degree(G):
    out_degree_dict = G.out_degree(G.nodes())
    for key in out_degree_dict.keys():
        if out_degree_dict[key] > 20:
            print('%d, %d' % (key, out_degree_dict[key]))


def visualize(G):
    nx.draw_circular(G)
    plt.show()


def save_topo(G):
    # save to topo.csv
    # and build an edge_map b.t.w.
    # key: edge_signature(source, dest, weight)
    # value: edge_id
    edge_map = {}
    cnt = 0
    with open(sys.argv[1], 'w') as topo_file:
        for (u, v, w) in G.edges(data='weight'):
            tl = str(cnt)+','+str(u)+','+str(v)+','+str(w)+'\n'
            topo_file.write(tl)
            key_tmp = (u, v, w)
            edge_map[key_tmp] = cnt
            # print ('%d, %d, %d, %d' % (cnt, u, v, w))
            cnt += 1
    return edge_map


def candi_filter(must_pass_candi):
    # randomly select 100 nodes
    must_pass = []
    while(len(must_pass)!=20):
        candi = random.choice(must_pass_candi)
        if candi not in must_pass:
            must_pass.append(candi)
    return must_pass


def save_demand(must_pass, source_node, dest_node):
    line_header = str(source_node) + ',' + str(dest_node) + ','
    first_demand = ''
    second_demand = ''
    for dmd in must_pass[0]:
        first_demand += str(dmd) + '|'
    for dmd in must_pass[1]:
        second_demand += str(dmd) + '|'
    with open(sys.argv[2], 'w') as demand_file:
        demand_file.write('1,' + line_header + first_demand[:-1] + '\n')
        demand_file.write('2,' + line_header + second_demand[:-1] + '\n')


def node2edge(edge_map, source, dest, nodes, weights):
    path = []
    prev_node = source
    w_cnt = 0
    for next_node in nodes:
        key_tmp = (prev_node, next_node, weights[w_cnt])
        path.append(edge_map[key_tmp])
        w_cnt += 1
        prev_node = next_node
    key_tmp = (prev_node, dest, weights[w_cnt])
    path.append(edge_map[key_tmp])
    return path


def save_ref(ref_path_one, ref_path_two):
    first_line = ''
    second_line = ''
    for node in ref_path_one:
        first_line += str(node) + '|'
    for node in ref_path_two:
        second_line += str(node) + '|'
    with open(sys.argv[3], 'w') as ref_file:
        ref_file.write(first_line[:-1] + '\n')
        ref_file.write(second_line[:-1] + '\n')


def main():
    if len(sys.argv) != 4:
        help_func()
        return

    # create networkx directed graph object
    G = nx.MultiDiGraph()

    # 2000 nodes with id 0, 1, 2, ..., 1999
    G.add_nodes_from(range(1900))

    # add two random edges
    source_node = random.choice(G.nodes())
    dest_node = random.choice(G.nodes())
    while(dest_node == source_node):  # make sure that source node not equal dest node
        dest_node = random.choice(G.nodes())
    visited = [source_node, dest_node]  # label visited nodes to make sure the path is acyclic
    must_pass_one_candi = []    # candidates of must pass nodes set one
    must_pass_one_weights = []
    must_pass_two_candi = []    # candidates of must pass nodes set two
    must_pass_two_weights = []
    prev_node_one = source_node
    prev_node_two = source_node
    for index in range(40):    # path 1
        next_node_one = random.choice(G.nodes())
        while(next_node_one in visited):
            next_node_one = random.choice(G.nodes())
        visited.append(next_node_one)
        must_pass_one_candi.append(next_node_one)
        tmp_weight = random.randint(1, 20)
        must_pass_one_weights.append(tmp_weight)
        G.add_edge(prev_node_one, next_node_one, weight = tmp_weight)
        prev_node_one = next_node_one
    tmp_weight = random.randint(1, 20)
    must_pass_one_weights.append(tmp_weight)
    G.add_edge(prev_node_one, dest_node, weight = tmp_weight)

    for index in range(40):    # path 2
        next_node_two = random.choice(G.nodes())
        while(next_node_two in visited):
            next_node_two = random.choice(G.nodes())
        visited.append(next_node_two)
        must_pass_two_candi.append(next_node_two)
        tmp_weight = random.randint(1, 20)
        must_pass_two_weights.append(tmp_weight)
        G.add_edge(prev_node_two, next_node_two, weight = tmp_weight)
        prev_node_two = next_node_two
    tmp_weight = random.randint(1, 20)
    must_pass_two_weights.append(tmp_weight)
    G.add_edge(prev_node_two, dest_node, weight = tmp_weight)


    # add random edges
    for node in G.nodes():
        for index in range(10):
            dest_tmp = random.choice(G.nodes())
            if dest_tmp != node and G.out_degree(node) < 5:
                w_tmp = random.randint(50, 100)
                G.add_edge(node, dest_tmp, weight=w_tmp)

    must_pass_one = candi_filter(must_pass_one_candi)
    must_pass_two = candi_filter(must_pass_two_candi)
    must_pass = [must_pass_one, must_pass_two]
    edge_map = save_topo(G)
    save_demand(must_pass, source_node, dest_node)
    ref_path_one = node2edge(edge_map, source_node, dest_node, must_pass_one_candi, must_pass_one_weights)
    ref_path_two = node2edge(edge_map, source_node, dest_node, must_pass_two_candi, must_pass_two_weights)
    save_ref(ref_path_one, ref_path_two)


if __name__ == '__main__':
    main()
