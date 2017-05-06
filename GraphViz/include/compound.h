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

        bool is_invalid() const;

        string to_string() const;

    private:
        unsigned int id;
};


// ID of a non-hierarchical connection between two nodes
class ConnId {
    friend class Compound;

    public:
        explicit ConnId(unsigned int id);
        string to_string() const;

    private:
        unsigned int id;
};

// A Point with two coordinates: x and y
class Point2D {
    public:
        Point2D(float x, float y): x(x), y(y) {}
        string to_string() const;

        float x;
        float y;

        Point2D operator*(const float &lhs)
        {
            Point2D result(this->x * lhs,this->y * lhs);
            return result;
        }

        Point2D operator+(const Point2D &lhs)
        {
            Point2D result(this->x + lhs.x, this->y + lhs.y);
            return result;
        }

        Point2D operator-(const Point2D &lhs)
        {
            Point2D result(this->x - lhs.x, this->y - lhs.y);
            return result;
        }
};

// an Element of a Tree
class TreeNode {
    friend class Compound;

    public:
        TreeNode(NodeId id, string label="");

        NodeId get_id() const;

        // returns an invalid NodeId if there is no parent, check with has_parent()
        NodeId get_parent_id() const;

        // only false for root node
        bool has_parent() const;

        // true if there are no children
        bool is_leaf() const;

        // possibly expensive as it copies the whole vector of children
        vector<NodeId> get_child_ids() const;

        const vector<NodeId>* get_const_child_ids() const;

        // possibly expensive as it copies the whole vector of connections
        vector<ConnId> get_connection_ids() const;

        // returns a string representation of the object
        string to_string() const;

        // get the level of the node, only the root node has level == 0
        unsigned int get_level() const;

        // get the number of children of this node and all its children
        unsigned int get_num_children() const;

        string get_label() const;
        void set_label(string label);

        Point2D get_position() const;
        void set_position(Point2D position);


    private:
        NodeId id;
        unsigned int level;
        NodeId parent_id;
        vector<NodeId> children;
        unsigned int num_children;
        string label;
        Point2D position;
        vector<ConnId> connections;
};


// a Path between nodes, possibly spanning over several nodes
class Path {
    public:
        string to_string() const;

        vector<NodeId> nodes;
};


// A compound graph class,
// representing inclusion and adjacency relations of its items
// using a tree (inclusion) and a vector of pairs (adjacency)
class Compound {
    public:
        Compound();
        virtual ~Compound();

        // create a random Compound graph
        static Compound create_random(unsigned int max_level,
                                      unsigned int num_nodes,
                                      unsigned int avg_num_children,
                                      unsigned int num_connections);

        string to_string() const;

        // get the id of the root node
        NodeId get_root_id() const;

        // construct a new node, add it to the tree and return its id
        NodeId add_node(NodeId parent, string label="");

        // return a pointer to a node in the tree
        TreeNode* get_node(NodeId id);

        // return a const pointer to a node in the tree
        const TreeNode* get_const_node(NodeId id) const;

        // add a connection between the nodes a and b
        void add_connection(NodeId a, NodeId b);

        // get all non hierarchical connections
        const vector<pair<NodeId, NodeId> >* get_connections();

        pair<NodeId, NodeId> get_connection(ConnId id) const;

        // return the shortest path between the nodes a and b
        Path get_shortest_path(NodeId a, NodeId b) const;

        // get the ids of all leaves
        vector<NodeId> get_leaf_ids() const;

    private:
        // nodes have an index of node.id - 1
        vector<TreeNode> nodes;

        // adjacency relations, each element represents a connection between two nodes.
        // Index of connections = ConnId.id
        vector<pair<NodeId, NodeId> > connections;
};


#endif
