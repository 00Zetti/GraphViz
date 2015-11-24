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

// A Point with two coordinates x and y
class Point2D {
    public:
        Point2D(float x, float y): x(x), y(y) {};
        string to_string();

        float x;
        float y;
};

// an Element of a Tree
class TreeNode {
    friend class Compound;

    public:
        TreeNode(NodeId id, string label="");

        NodeId get_id();

        // returns an invalid NodeId if there is no parent, check with has_parent()
        NodeId get_parent_id();

        // only false for root node
        bool has_parent();

        // possibly expensive as it copies the whole vector of children
        vector<NodeId> get_child_ids();

        // possibly expensive as it copies the whole vector of connections
        vector<ConnId> get_connection_ids();

        // returns a string representation of the object
        string to_string();

        // get the level of the node, only the root node has level == 0
        unsigned int get_level();

        // get the number of leaves of this node and all its children
        unsigned int get_num_leaves();

        string get_label();
        void set_label(string label);

        Point2D get_position();
        void set_position(Point2D position);


    private:
        NodeId id;
        unsigned int level;
        NodeId parent_id;
        vector<NodeId> children;
        unsigned int num_leaves;
        string label;
        Point2D position;
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
