#include "../include/compound.h"

#include <iostream>
#include <sstream>
using namespace std;


NodeId::NodeId(unsigned int id): id(id) {
}

NodeId NodeId::invalid() {
    return NodeId(0);
}

bool NodeId::is_invalid() {
    return id == 0;
}

string NodeId::to_string() {
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

string ConnId::to_string() {
    stringstream s;
    s << "ConnId(" << id << ")";

    return s.str();
}


TreeNode::TreeNode(NodeId id, string label):
    id(id),
    level(0),
    parent_id(NodeId::invalid()),
    children(vector<NodeId>()),
    label(label),
    connections(vector<ConnId>()) {
}

NodeId TreeNode::get_id() {
    return id;
}

NodeId TreeNode::get_parent_id() {
    return parent_id;
}

bool TreeNode::has_parent() {
    return parent_id.id != 0;
}

string TreeNode::to_string() {
    stringstream s;
    s << "TreeNode(id: " << id.to_string();
    s << ", level: " << level;
    s << ", label: \"" << label << "\"";
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

string TreeNode::get_label() {
    return label;
}

void TreeNode::set_label(string label) {
    this->label = label;
}


string Path::to_string() {
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


NodeId Compound::get_root_id() {
    return nodes.at(0).get_id();
}


NodeId Compound::add_node(NodeId parent_id, string label) {
    NodeId id = NodeId(nodes.size() + 1);

    TreeNode n = TreeNode(id, label);
    n.parent_id = parent_id;
    n.level = get_node(parent_id)->level + 1;
    nodes.push_back(n);

    // add node to parents list of children
    get_node(parent_id)->children.push_back(id);

    return id;
}

TreeNode* Compound::get_node(NodeId id) {
    return &nodes.at(id.id - 1);
}

void Compound::add_connection(NodeId a, NodeId b) {
    ConnId id = ConnId(connections.size());
    connections.push_back(pair<NodeId, NodeId>(a, b));

    // inform the two nodes about this
    get_node(a)->connections.push_back(id);
    get_node(b)->connections.push_back(id);
}


pair<NodeId, NodeId> Compound::get_connection(ConnId id) {
    return connections.at(id.id);
}


Path Compound::get_shortest_path(NodeId a, NodeId b) {
    Path path;
    TreeNode *x = get_node(a);
    TreeNode *y = get_node(b);

    // move up from node a
    while (x->level > y->level) {
        path.nodes.push_back(x->id);
        if (x->parent_id.is_invalid()) {
            break;
        } else {
            x = get_node(x->parent_id);
        }
    }

    // move up from node b
    vector<NodeId> tail;
    while (y->level > x->level) {
        tail.push_back(y->id);
        if (y->parent_id.is_invalid()) {
            break;
        } else {
            y = get_node(y->parent_id);
        }
    }

    // move up together until we reach a common node
    while (x->id.id != y->id.id) {
        path.nodes.push_back(x->id);
        tail.push_back(y->id);
        if (x->parent_id.is_invalid() || y->parent_id.is_invalid()) {
            break;
        } else {
            x = get_node(x->parent_id);
            y = get_node(y->parent_id);
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
