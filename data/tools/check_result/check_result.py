#!/usr/bin/env python
# encoding: utf-8

import sys


def help_func():
    print('Check resulted path')
    print('\tUsage:')
    print('\t./main.py topo.csv demand.csv result.csv')


def main():
    if len(sys.argv) != 4:
        help_func()
        return
    topo_map = {}
    source_id = -1
    dest_id = -1
    demand_one = set()
    demand_two = set()
    result_one = []
    result_two = []
    with open(sys.argv[1], 'r') as topo_file:
        for topo_line in topo_file.readlines():
            if topo_line.find('\n') == -1:
                topo_infos = topo_line.split(',')
            else:
                topo_infos = topo_line[:-1].split(',')
            topo_map[topo_infos[0]] = [topo_infos[1], topo_infos[2], topo_infos[3]]

    with open(sys.argv[2], 'r') as demand_file:
        first_line = demand_file.readline()
        second_line = demand_file.readline()
        if first_line.find('\n') == -1:
            first_infos = first_line.split(',')
        else:
            first_infos = first_line[:-1].split(',')
        if second_line.find('\n') == -1:
            second_infos = second_line.split(',')
        else:
            second_infos = second_line[:-1].split(',')
        source_id = first_infos[1]
        dest_id = first_infos[2]
        demand_one_infos = first_infos[3]
        demand_two_infos = second_infos[3]
        demand_one = set(demand_one_infos.split('|'))
        demand_two = set(demand_two_infos.split('|'))

    with open(sys.argv[3], 'r') as result_file:
        first_line = result_file.readline()
        second_line = result_file.readline()
        if first_line.find('\n') == -1:
            result_one = first_line.split('|')
        else:
            result_one = first_line[:-1].split('|')
        if second_line.find('\n') == -1:
            result_two = second_line.split('|')
        else:
            result_two = second_line[:-1].split('|')

    is_connect = True
    start_one = topo_map[result_one[0]][0]
    start_two = topo_map[result_two[0]][0]
    end_one = topo_map[result_one[-1]][1]
    end_two = topo_map[result_two[-1]][1]
    if start_one != source_id or start_two != source_id:
        is_connect = False
        print('Start point not right!')
    if end_one != dest_id or end_two != dest_id:
        is_connect = False
        print('End point not right!')
    prev_one = start_one
    prev_two = start_two
    prev_lk = result_one[0]
    for lk in result_one:
        if topo_map.get(lk) == None:
            is_connect = False
            print('PathOne: Link ID not found!')
            break
        if topo_map[lk][0] != prev_one:
            is_connect = False
            print('PathOne: Link not continious! With: ')
            print('LinkID: %s, (%s, %s) and LinkID: %s,(%s, %s)' % (prev_lk, topo_map[prev_lk][0], topo_map[prev_lk][1], lk, topo_map[lk][0], topo_map[lk][1]))
            break
        prev_one = topo_map[lk][1]
        prev_lk = lk

    prev_lk = result_two[0]
    for lk in result_two:
        if topo_map.get(lk) == None:
            is_connect = False
            print('PathTwo: Link ID %s not found!' % lk)
            break
        if topo_map[lk][0] != prev_two:
            is_connect = False
            print('PathTwo: Link not continious! With ')
            print('LinkID: %s, (%s, %s) and LinkID: %s,(%s, %s)' % (prev_lk, topo_map[prev_lk][0], topo_map[prev_lk][1], lk, topo_map[lk][0], topo_map[lk][1]))
            break
        prev_two = topo_map[lk][1]
        prev_lk = lk

    if is_connect:
        print('Connectivity Check Pass!')
    else:
        print('Connectivity Check not Pass!')

    is_acyclic = True
    visited_one = {}
    visited_two= {}
    visited_one[start_one] = True
    visited_two[start_one] = True
    for lk in result_one:
        if visited_one.get(topo_map[lk][1]) != None:
            is_acyclic = False
            print('PathOne: Revisit Node: %s' % topo_map[lk][1])
            break
        else:
            visited_one[topo_map[lk][1]] = True
    for lk in result_two:
        if visited_two.get(topo_map[lk][1]) != None:
            is_acyclic = False
            print('PathTwo: Revisit Node: %s' % topo_map[lk][1])
            break
        else:
            visited_two[topo_map[lk][1]] = True
    if is_acyclic:
        print('Acyclic Property Check Pass!')
    else:
        print('Acyclic Property Check Not Pass!')

    demand_one.add(start_one)
    demand_one.add(end_one)
    demand_two.add(start_two)
    demand_two.add(end_two)
    for lk in result_one:
        if topo_map[lk][0] in demand_one:
            demand_one.remove(topo_map[lk][0])
        if topo_map[lk][1] in demand_one:
            demand_one.remove(topo_map[lk][1])
    for lk in result_two:
        if topo_map[lk][0] in demand_two:
            demand_two.remove(topo_map[lk][0])
        if topo_map[lk][1] in demand_two:
            demand_two.remove(topo_map[lk][1])

    if len(demand_one) == 0 and len(demand_two) == 0:
        print('Msut Pass Check Pass!')
    elif len(demand_one) != 0:
        print('Must Pass Check Failed! With nodes not pass in pathOne: ')
        nodes_str = ''
        for dmd in demand_one:
            nodes_str += dmd + ' '
        print('\t' + nodes_str)
    elif len(demand_two) != 0:
        print('Must Pass Check Failed! With nodes not pass in pathTwo: ')
        nodes_str = ''
        for dmd in demand_two:
            nodes_str += dmd + ' '
        print('\t' + nodes_str)


if __name__ == '__main__':
    main()
