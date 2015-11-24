#ifndef COMPOUND_H
#define COMPOUND_H

#include <vector>
#include <string>
#include <utility>

using namespace std;


// ID of a Node, basically a glorified index
class NodeId {
    friend class TreeNode;
    friend class Compound;

    public:
        explicit NodeId(unsigned int id);

        // returns an invalid NodeId
        static NodeId invalid();

        bool is_invalid();

        string to_string();

    private:
        unsigned int id;
};


// ID of a non-hierarchical connection between two nodes
class ConnId {
    friend class Compound;

    public:
        explicit ConnId(unsigned int id);
        string to_string();

    private:
        unsigned int id;
};


// an Element of a Tree
class TreeNode {
    friend class Compound;

    public:
        TreeNode(NodeId id, string label="");

        NodeId get_id();

        // returns an invalid NodeId if there is no parent, check with has_parent()
        NodeId get_parent_id();

        bool has_parent();

        string to_string();

        string get_label();
        void set_label(string label);

    private:
        NodeId id;
        unsigned int level;
        NodeId parent_id;
        vector<NodeId> children;
        string label;
        vector<ConnId> connections;
};


// a Path between nodes, possibly spanning over several nodes
class Path {
    public:
        string to_string();

        vector<NodeId> nodes;
};


// A compound graph class,
// representing inclusion and adjacency relations of its items
// using a tree (inclusion) and a vector of pairs (adjacency)
class Compound {
    public:
        Compound();
        virtual ~Compound();

        // get the id of the root node
        NodeId get_root_id();

        // construct a new node, add it to the tree and return its id
        NodeId add_node(NodeId parent, string label="");

        // return a pointer to a node in the tree
        TreeNode* get_node(NodeId id);

        // add a connection between the nodes a and b
        void add_connection(NodeId a, NodeId b);

        pair<NodeId, NodeId> get_connection(ConnId id);

        // return the shortest path between the nodes a and b
        Path get_shortest_path(NodeId a, NodeId b);

    private:
        // nodes have an index of node.id - 1
        vector<TreeNode> nodes;

        // adjacency relations, each element represents a connection between two nodes.
        // Index of connections = ConnId.id
        vector<pair<NodeId, NodeId> > connections;
};


#endif
