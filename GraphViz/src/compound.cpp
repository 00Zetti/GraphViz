#include "../include/compound.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <sstream>
using namespace std;


NodeId::NodeId(unsigned int id): id(id) {
}

NodeId NodeId::invalid() {
    return NodeId(0);
}

bool NodeId::is_invalid() const {
    return id == 0;
}

string NodeId::to_string() const {
    stringstream s;
    s << "NodeId(";
    if (is_invalid()) {
        s << "Invalid";
    } else {
        s << id;
    }
    s << ")";

    return s.str();
}


ConnId::ConnId(unsigned int id): id(id) {
}

string ConnId::to_string() const {
    stringstream s;
    s << "ConnId(" << id << ")";

    return s.str();
}


string Point2D::to_string() const {
    stringstream s;
    s << "Point2D(" << x << ", " << y << ")";

    return s.str();
}


TreeNode::TreeNode(NodeId id, string label):
    id(id),
    level(0),
    parent_id(NodeId::invalid()),
    children(vector<NodeId>()),
    num_children(0),
    label(label),
    position(Point2D(0, 0)),
    connections(vector<ConnId>()) {
}

NodeId TreeNode::get_id() const {
    return id;
}

NodeId TreeNode::get_parent_id() const {
    return parent_id;
}

bool TreeNode::has_parent() const {
    return parent_id.id != 0;
}

bool TreeNode::is_leaf() const {
    return children.empty();
}

vector<NodeId> TreeNode::get_child_ids() const {
    return children;
}

vector<ConnId> TreeNode::get_connection_ids() const {
    return connections;
}

string TreeNode::to_string() const {
    stringstream s;
    s << "TreeNode(id: " << id.to_string();
    s << ", level: " << level;
    s << ", num_leaves: " << num_children;
    s << ", label: \"" << label << "\"";
    s << ", position: " << position.to_string();
    s << ", parent: " << parent_id.to_string();
    s << ", children: [";
    for (unsigned int i = 0; i < children.size(); i++) {
        s << children[i].to_string();
        if (i < children.size() - 1) {
            s << ", ";
        }
    }
    s << "], connections: [";
    for (unsigned int i = 0; i < connections.size(); i++) {
        s << connections[i].to_string();
        if (i < connections.size() - 1) {
            s << ", ";
        }
    }
    s << "])";

    return s.str();
}

unsigned int TreeNode::get_level() const {
    return level;
}

unsigned int TreeNode::get_num_children() const {
    return num_children;
}

string TreeNode::get_label() const {
    return label;
}

void TreeNode::set_label(string label) {
    this->label = label;
}

Point2D TreeNode::get_position() const {
    return position;
}

void TreeNode::set_position(Point2D position) {
    this->position = position;
}


string Path::to_string() const {
    stringstream s;
    s << "Path(nodes: [";
    for (unsigned int i = 0; i < nodes.size(); i++) {
        s << nodes[i].to_string();
        if (i < nodes.size() - 1) {
            s << ", ";
        }
    }
    s << "])";

    return s.str();
}


Compound::Compound() {
    // add root node
    nodes.push_back(TreeNode(NodeId(1)));
}

Compound::~Compound() {

}


Compound Compound::create_random(unsigned int max_level,
                                 unsigned int min_num_nodes,
                                 unsigned int avg_num_children,
                                 unsigned int num_connections) {

    Compound com;

    if (max_level < 1) {
        cerr << "max_level should be >= 1" << endl;
        return com;
    }

    if (avg_num_children < 2) {
        cerr << "avg_num_children should be >= 2" << endl;
        return com;
    }

    // Initialize random number generator.
    // This should ideally be done only once at program start.
    srand(time(NULL));

    NodeId x = com.get_root_id();
    while (com.nodes.size() < min_num_nodes) {

        // move down
        while (com.get_const_node(x)->level < max_level) {
            x = com.add_node(x);
        }

        // move up
        x = com.get_const_node(x)->parent_id;
        for (int k = 0; k < max_level - 1; k++) {
            if (rand() % avg_num_children != 0) {
                break;
            }
            x = com.get_const_node(x)->parent_id;
        }
    }

    vector<NodeId> leaves = com.get_leaf_ids();
    unsigned int num_leaves = leaves.size();

    if (num_leaves <= 1) {
        cerr << "there are not enough leaves to add connections." << endl;
        return com;
    }

    while (com.connections.size() < num_connections) {
        unsigned int a = rand() % num_leaves;
        unsigned int b = rand() % num_leaves;
        if (a != b) {
            com.add_connection(leaves[a], leaves[b]);
        }
    }

    return com;
}

string Compound::to_string() const {
    stringstream s;
    s << "Compound(nodes: [\n";
    for (unsigned int i = 0; i < nodes.size(); i++) {
        s << nodes[i].to_string();
        if (i < nodes.size() - 1) {
            s << ",\n";
        }
    }
    s << "])";

    return s.str();
}


NodeId Compound::get_root_id() const {
    return nodes.at(0).get_id();
}


NodeId Compound::add_node(NodeId parent_id, string label) {
    NodeId id = NodeId(nodes.size() + 1);

    TreeNode n = TreeNode(id, label);
    n.parent_id = parent_id;
    n.level = get_node(parent_id)->level + 1;
    nodes.push_back(n);

    // add node to parent's list of children
    get_node(parent_id)->children.push_back(id);

    // increment num_children of all parents
    {
        TreeNode *x = get_node(id);
        while (x->has_parent()) {
            x = get_node(x->parent_id);

            x->num_children += 1;
        }
    }

    return id;
}

TreeNode* Compound::get_node(NodeId id) {
    return &nodes.at(id.id - 1);
}

const TreeNode* Compound::get_const_node(NodeId id) const {
    return &nodes.at(id.id - 1);
}

void Compound::add_connection(NodeId a, NodeId b) {
    ConnId id = ConnId(connections.size());
    connections.push_back(pair<NodeId, NodeId>(a, b));

    // inform the two nodes about this
    get_node(a)->connections.push_back(id);
    get_node(b)->connections.push_back(id);
}

const vector<pair<NodeId, NodeId> >* Compound::get_connections() {
    return &connections;
}

pair<NodeId, NodeId> Compound::get_connection(ConnId id) const {
    return connections.at(id.id);
}


Path Compound::get_shortest_path(NodeId a, NodeId b) const {
    Path path;
    TreeNode const *x = get_const_node(a);
    TreeNode const *y = get_const_node(b);

    // move up from node a
    while (x->level > y->level) {
        path.nodes.push_back(x->id);
        if (x->parent_id.is_invalid()) {
            break;
        } else {
            x = get_const_node(x->parent_id);
        }
    }

    // move up from node b
    vector<NodeId> tail;
    while (y->level > x->level) {
        tail.push_back(y->id);
        if (y->parent_id.is_invalid()) {
            break;
        } else {
            y = get_const_node(y->parent_id);
        }
    }

    // move up together until we reach a common node
    while (x->id.id != y->id.id) {
        path.nodes.push_back(x->id);
        tail.push_back(y->id);
        if (x->parent_id.is_invalid() || y->parent_id.is_invalid()) {
            break;
        } else {
            x = get_const_node(x->parent_id);
            y = get_const_node(y->parent_id);
        }
    }

    if (x->id.id == y->id.id) {
        path.nodes.push_back(x->id);

        // add tail to path, in reverse order
        for (unsigned int i = 0; i < tail.size(); i++) {
            path.nodes.push_back(tail[tail.size() - i - 1]);
        }

        return path;
    } else {
        // There is no possible path -> return empty Path
        return Path();
    }
}

vector<NodeId> Compound::get_leaf_ids() const {
    vector<NodeId> leaves;

    for (unsigned int i = 0; i < nodes.size(); i++) {
        const TreeNode *n = &nodes[i];
        if (n->is_leaf()) {
            leaves.push_back(n->id);
        }
    }

    return leaves;
}

