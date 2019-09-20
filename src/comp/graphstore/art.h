/*
 * art.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_ART_H_
#define COMP_GRAPHSTORE_ART_H_

#include "graphstore.h"

using namespace std;

struct Edge {
    int src;
    int des;
    char label;

    Edge() {
        src = -1;
        label = -1;
        des = -1;
    };

    Edge(int src, char label, int des) : src(src), label(label), des(des) {};

    Edge(const Edge &edge) {                //拷贝构造函数
        this->src = edge.src;
        this->des = edge.des;
        this->label = edge.label;
    }

    bool equal(Edge *other) {               // 判断是够相等
        if (!other) return false;
        return this->src == other->src && this->des == other->des && this->label == other->label;
    }

    bool operator==(const Edge &rhs) const {
        return src == rhs.src &&
               des == rhs.des &&
               label == rhs.label;
    }


    bool operator!=(const Edge &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Edge &rhs) const {
        if (src < rhs.src)
            return true;
        if (rhs.src < src)
            return false;
        if (des < rhs.des)
            return true;
        if (rhs.des < des)
            return false;
        return label < rhs.label;
    }

    bool operator>(const Edge &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Edge &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Edge &rhs) const {
        return !(*this < rhs);
    }

    friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
        os << "src: " << edge.src << " des: " << edge.des << " label: " << edge.label;
        return os;
    }
};

// declare hash<Edge>
namespace std {
    template<>
    struct hash<Edge> {
        std::size_t operator()(const Edge &e) const {
            using std::hash;
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:
            return ((hash<int>()(e.src)
                     ^ (hash<int>()(e.des) << 1)) >> 1)
                   ^ (hash<char>()(e.label) << 1);
        }
    };

}


struct Node {
    Edge edge;
    Node *parent;
    vector<Node*> children;

    Node() {
        parent = nullptr;
    }

    bool equal(Node* anotherNode) {
        if (!anotherNode)return false;
        return this->edge.equal(&anotherNode->edge);
    }

    bool equal(Edge *anotherEdge) {
        if (!anotherEdge)return false;
        return this->edge.equal(anotherEdge);
    }

    void toString() {
        cout << this->edge.src << " " << this->edge.label << " " << this->edge.des << endl;
    }

    void insert_binarySearch(Node *child){
        this->children.insert(lower_bound(this->children.begin(), this->children.end(), child), child);
    }
};

class ART : public GraphStore {

public:
    ART();

    ~ART() override;

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    Node *insert(vector<Edge> &vector_edge);

    vector<Edge> retrieveFromLeaf(Node *node) const;

    void deleteSingleGraph(Node *leaf);

    void edgeSort(vector<Edge> &vector_edge);

    Node *findChild(Node *parent, Edge *child);

    PEGraph * convertToPEGraph(vector<Edge> &vector_edge) const;

    vector<Edge> convertToVector(PEGraph *peGraph);


    string toString() override;

protected:
    void print(std::ostream &str) override;

    void toString_sub(std::ostringstream &strm) override;

public:
    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) override;

    void update_graphs(GraphStore *another) override;

    void clearEntryOnly() override;

    void clear() override;

    void loadGraphStore(const string &file, const string &file_in) override;

private:
    Node *root;
    std::unordered_map<PEGraph_Pointer, Node *> mapToLeafNode;
    unordered_map<Node*, int> mapToLeafNum;
    unordered_map<Edge, int> sortBase;

    void postOrderDelete_iteration(Node *root);
    void _clear(Node *node);

    void update_graphs_parallel(GraphStore *another);

    void update_graphs_sequential(GraphStore *another);

    void deserialize(const string &file);

    void serialize(const string &file);



    void load_onebyone(const string &file);
};


#endif /* COMP_GRAPHSTORE_ART_H_ */