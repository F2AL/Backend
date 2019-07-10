//
// Created by guihang on 2019/6/9.
//
#include "art.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

ART::ART(){
	//TODO
}

ART::~ART() {
    clear();
}

string ART::toString() {
    return GraphStore::toString();
}

void ART::print(std::ostream &str) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    str << "The number of graphs is: " << mapToLeaf.size() << "\n";
    for(auto & it : mapToLeaf){
        vector<Edge *> vector= retrieveFromLeaf(it.second);
        PEGraph* peGraph = convertToPEGraph(vector);
        str << ">>>>" << it.first << " " << *peGraph << endl;
    }
}

void ART::toString_sub(std::ostringstream &str) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    str << "The number of graphs is: " << mapToLeaf.size() << "\n";
    for(auto & it : mapToLeaf){
        vector<Edge *> vector= retrieveFromLeaf(it.second);
        PEGraph* peGraph = convertToPEGraph(vector);
        str << ">>>>" << it.first << " " << *peGraph << endl;
    }
}

Node *ART::insert(vector<Edge *> &v) {
    if (v.empty()) return nullptr;
    Node *parent = root;
    for (auto &i : v) {
        Node *child = findChild(parent, i);
        if (!child) {
            child = new Node();
            child->data = new Edge(*i);
            child->parent = parent;
            // insert new child to the front of the children
            child->next = parent->children;
            parent->children = child;
        }
        parent = child;
    }
    parent->leafNum++;
    return parent;
}

void ART::addOneGraph(PEGraph_Pointer pointer, PEGraph *graph) {
    Node* leaf = insertNewGraph(graph);
    this->mapToLeaf[pointer] = leaf;
}

void ART::update_graphs(GraphStore *another) {
    ART* another_graphstore = dynamic_cast<ART*>(another);
    for(auto& it: another_graphstore->mapToLeaf){
        update(it.first, retrieve(it.first));
    }
}

void ART::clearEntryOnly() {

}

void ART::clear() {
    clear_dfs(root, root);
    //todo 需不需要释放map要看我是浅拷贝还是深拷贝

}

vector<Edge *> ART::retrieveFromLeaf(Node *node) const {
    vector<Edge *> v;
    while (node->data != nullptr) {
        v.push_back(node->data);
        node = node->parent;
    }
    return v;
}

void ART::del(Node *leaf) {
    if (leaf == nullptr)
        return;
    Node *node = leaf->parent;

    // 要删除的图只有一个叶子结点并且叶子结点没有孩子
    if (leaf->leafNum == 1 && leaf->children == nullptr) {

        if (leaf->parent->children->equal(leaf)) {                  // 如果待删除的是头结点
            if (leaf->next) {                                           //如果有后继
                leaf->parent->children = leaf->next;
            } else {                                                    // 如果没有后继
                leaf->parent->children = nullptr;
            }

            delete leaf;
        } else {                                                    // 如果待删除的不是头结点
            Node *before = leaf->parent->children;                      //找到待删除的之前的结点
            while (!before->next->equal(leaf)) {
                before = before->next;
            }
            before->next = leaf->next;
            delete leaf;
        }
    } else {                                                        // 如果leaf大于1 或者Leaf还有孩子，LeafNum - 1
        leaf->leafNum--;
        return;
    }

    while (node->parent && node->leafNum == 0 && node->children == nullptr) {
        Node *temp = node;
        if (node->parent->children->equal(node)) {
            if (node->next) {
                node->parent->children = node->next;
            } else {
                node->parent->children = nullptr;
            }
            delete node;
        } else {
            Node *before = node->parent->children;
            while (!before->next->equal(node)) {
                before = before->next;
            }
            before->next = node->next;
            delete node;
        }
        node = temp->parent;
    }
}

void ART::clear_dfs(Node *node, Node *head) {
    // node is the root
    if (node->children) {
        clear_dfs(node->children, node->children);
    }
    if (node->next) {
        clear_dfs(node->next, head);          //head保持不变
    }
    //对一整行作析构
    clearLinkList(head);
}



Node *ART::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    Node *children = parent->children;
    while (children) {
        if (children->equal(child)) return children;
        children = children->next;
    }
    return nullptr;
}

PEGraph * ART::convertToPEGraph(vector<Edge *> &graph) const {
    auto* peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> mapToEdgeArray;

    sort(graph.begin(),graph.end(), cmp);
    for(auto & edge : graph){
        vertexid_t src = edge->src;
        vertexid_t dst = edge->dst;
        label_t label = edge->label;
        if(mapToEdgeArray.find(src) == mapToEdgeArray.end()){
            mapToEdgeArray[src] = EdgeArray();
        }
        mapToEdgeArray[src].addOneEdge(dst, label);
    }

    peGraph->setGraph(mapToEdgeArray);
    return peGraph;
}

vector<Edge *> ART::convertToVector(PEGraph *peGraph) {
    vector<Edge *> edgeVector;
    for(auto & it : peGraph->getGraph()){
//        Edge * edge = new Edge()
        int size = it.second.getSize();
        vertexid_t* edges = it.second.getEdges();
        label_t* labels = it.second.getLabels();
        Edge* edge;
        for (int i = 0; i < size; ++i) {
            edge = new Edge(it.first, edges[i], labels[i]);
            edgeVector.push_back(edge);
            delete edge;
        }
    }
    return edgeVector;
}

void ART::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, pegraph);
}
void ART::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    Node *node = mapToLeaf[graph_pointer];
    del(node);
    vector<Edge *> vector_graph = convertToVector(pegraph);
    // todo we can sort vector_graph before insert
    sort(vector_graph.begin(),vector_graph.end(), cmp);
    Node *leaf = insert(vector_graph);
    mapToLeaf[graph_pointer] = leaf;
}

PEGraph * ART::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph * ART::retrieve(PEGraph_Pointer graph_pointer) {
    if (mapToLeaf.find(graph_pointer)!= mapToLeaf.end()){
        Node* leaf = mapToLeaf[graph_pointer];
        vector<Edge *> v = retrieveFromLeaf(leaf);
        return convertToPEGraph(v);             /*convert from vector<Edge*> to PEGraph* */
    }
    return new PEGraph();
}

bool ART::cmp(Edge* a, Edge* b){
    return sortBase[*a] > sortBase[*b];
}

void ART::edgeSort(vector<vector<Edge *>> &graphs) {

    for (auto &graph : graphs) {
        for (auto &edge : graph) {
            if (sortBase.find(*edge) == sortBase.end()) {
                sortBase[*edge] = 1;
            } else {
                sortBase[*edge]++;
            }
        }
    }
    for (auto &graph : graphs) {
        sort(graph.begin(),graph.end(), cmp);
    }
}

Node *ART::insertNewGraph(PEGraph *pGraph) {
    vector<Edge*> edgeVector=convertToVector(pGraph);
    Node* leaf = insert(edgeVector);
    return leaf;
}

void ART::loadGraphStore(const string &file) {

}

void ART::clearLinkList(Node *head) {
    if(head == root){
        delete(head);
        return;
    }
//    head->parent->children = nullptr;           //todo 1.考虑root 2.置为null和直接delete的区别
    delete head->parent->children;
    Node *temp;
    while(head->next!=nullptr)
    {
        temp=head->next;
        delete head;
        head=temp;
    }
    delete head;
}

