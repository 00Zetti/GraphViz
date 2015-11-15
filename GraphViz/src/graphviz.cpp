#include "../include/compound.h"

#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
    Compound com;

    NodeId root = com.get_root_id();
    com.get_node(root)->set_label("Root");

    // add a child to root and label it
    NodeId a = com.add_node(root, "A");

    // some other nodes
    NodeId b = com.add_node(root, "B");
    NodeId c = com.add_node(b, "C");

    // add non-hierarchical connections
    com.add_connection(a, c);
    com.add_connection(a, root);

    // print the nodes
    cout << com.get_node(root)->to_string() << endl;
    cout << com.get_node(a)->to_string() << endl;
    cout << com.get_node(b)->to_string() << endl;
    cout << com.get_node(c)->to_string() << endl;

    // compute and print shortest path between a and c
    Path p = com.get_shortest_path(a, c);
    cout << p.to_string() << endl;

    return 0;
}
