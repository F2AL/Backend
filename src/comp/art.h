/*
 * art.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_ART_H_
#define COMP_ART_H_

#include "graphstore.h"

using namespace std;

struct Edge {
    int src;
    int dst;
    char label;

    Edge() {};

    Edge(int src, char label, int des) : src(src), label(label), dst(des) {};

    Edge(const Edge &edge) {
        this->src = edge.src;
        this->dst = edge.dst;
        this->label = edge.label;
    }

    bool equal(Edge *other) {
        if (!other) return false;
        return this->src == other->src && this->dst == other->dst && this->label == other->label;
    }

    bool operator==(const Edge &rhs) const {
        return src == rhs.src &&
               dst == rhs.dst &&
               label == rhs.label;
    }

    bool operator<(const Edge &rhs) const {
        if (src < rhs.src)
            return true;
        if (rhs.src < src)
            return false;
        if (dst < rhs.dst)
            return true;
        if (rhs.dst < dst)
            return false;
        return label < rhs.label;
    }



    bool operator!=(const Edge &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
        os << "src: " << edge.src << " dst: " << edge.dst << " label: " << edge.label;
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
                     ^ (hash<int>()(e.dst) << 1)) >> 1)
                   ^ (hash<char>()(e.label) << 1);
        }
    };

}


struct Node {
    Edge *data;
    Node *parent;
    Node *children;
    Node *next;
    int leafNum;

    Node() : parent(nullptr), children(nullptr), data(nullptr), next(nullptr), leafNum(0) {};

    bool equal(Node *other) {
        if (!other)return false;
        return this->data->equal(other->data);
    }

    bool equal(Edge *other) {
        if (!other)return false;
        return this->data->equal(other);
    }

    void toString() {
        cout << this->data->src << " " << this->data->label << " " << this->data->dst << endl;
    }
};

class ART : public GraphStore {

public:
    ART();

    ~ART();

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    Node *insert(vector<Edge *> &v);

    vector<Edge *> retrieveFromLeaf(Node *node) const;

    static void del(Node *leaf);

    void clear_dfs(Node *node, Node *head);

     void edgeSort(vector<vector<Edge *>> &graphs);

    Node *findChild(Node *parent, Edge *child);

    PEGraph * convertToPEGraph(vector<Edge *> &graph) const;

    static vector<Edge *> convertToVector(PEGraph *peGraph);

    void loadGraphStore(const string &file) override;

    string toString() override;

    void addOneGraph(PEGraph_Pointer pointer, PEGraph *graph) override;

    void update_graphs(GraphStore *another) override;

    void clearEntryOnly() override;

    void clear() override;

protected:
    void print(std::ostream &str) override;

    void toString_sub(std::ostringstream &str) override;

//    void addOneSingleton(vertexid_t t);

private:
    Node *root = new Node();
    std::unordered_map<PEGraph_Pointer, Node *> mapToLeaf;
    static unordered_map<Edge, int> sortBase;
//    std::set<vertexid_t> singletonSet;

    Node *insertNewGraph(PEGraph *pGraph);

    void clearLinkList(Node *head);

    static bool cmp(Edge *a, Edge *b);
};


#endif /* COMP_ART_H_ */
