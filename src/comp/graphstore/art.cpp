//
// Created by guihang on 2019/6/9.
//
#include "art.h"

#include <unordered_map>
#include <algorithm>
#include <FileUtil.hpp>

using namespace std;

ART::ART(){
    root = new Node();
}

ART::~ART() {
    clear();
}


string ART::toString() {
    return GraphStore::toString();
}

void ART::print(std::ostream &str) {

}

void ART::toString_sub(std::ostringstream &strm) {

}

Node *ART::insert(vector<Edge> &vector_edge) {
    if (vector_edge.empty()) return nullptr;
    Node *parent = root;
    for (auto &edge : vector_edge) {
        Node *child = findChild(parent, &edge);
        if (!child) {
            child = new Node();
            child->edge = Edge(edge);           //##这是深拷贝还是浅拷贝?
            child->parent = parent;
            parent->insert_binarySearch(child);
        }
        parent = child;
    }
//    parent->leafNum++;
    mapToLeafNum[parent]++;
    return parent;
}

vector<Edge> ART::retrieveFromLeaf(Node *node) const {
    vector<Edge> v;
    while (node != nullptr) {         //这里被取代之后该如何操作?
        v.push_back(node->edge);
        node = node->parent;
    }
    return v;
}

void ART::deleteSingleGraph(Node *leaf) {
    if(leaf == nullptr) return;

    //  如果leaf是root,直接删除并返回
    if (leaf->parent == nullptr){
        delete leaf;
        return;
    }
    // 如果待删除的图不止一个
    if (mapToLeafNum[leaf] > 1) {
        mapToLeafNum[leaf]--;
        return;
    }

    // 如果后面还有孩子
    if (mapToLeafNum[leaf] == 1 && !leaf->children.empty()) {
        mapToLeafNum[leaf]--;
        return;
    }

    //如果后面没有孩子
    mapToLeafNum[leaf]--;
    Node *parent = leaf->parent;
    while (parent && leaf->children.empty()) {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), leaf), parent->children.end());
        delete leaf;
        leaf = parent;
        parent = leaf->parent;
    }
}

//使用二分查找寻找child
Node *ART::findChild(Node *parent, Edge *child) {
    if (!parent || !child) return nullptr;
    vector<Node *> children = parent->children;
    int left = 0, right = children.size() - 1;
    int mid;
    while (left <= right) {
        mid = (left + right) / 2;
        if (children[mid]->edge == *child)
            return children[mid];
        else if (children[mid]->edge > *child)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return nullptr;
}

PEGraph * ART::convertToPEGraph(vector<Edge> &vector_edge) const {
    PEGraph* peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;

    for(auto & edge : vector_edge){
        vertexid_t src = edge.src;
        if(graph.find(src) == graph.end()){
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(edge.des, edge.label);

    }

    peGraph->setGraph(graph);
    return peGraph;
}

vector<Edge> ART::convertToVector(PEGraph *peGraph) {
    vector<Edge> edgeVector;
    unordered_map<vertexid_t, EdgeArray> &map = peGraph->getGraph();
    for(auto & it : map){
//        Edge * edge = new Edge()
        int size = it.second.getSize();
        vertexid_t* edges = it.second.getEdges();
        label_t* labels = it.second.getLabels();
        Edge edge;
        for (int i = 0; i < size; ++i) {
            edge =  Edge(it.first, edges[i], labels[i]);
            edgeVector.push_back(edge);
        }
    }
    return edgeVector;
}

void ART::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, pegraph);
}
void ART::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    Node *node = mapToLeafNode[graph_pointer];
    deleteSingleGraph(node);
    auto v = convertToVector(pegraph);
    Node *leaf = insert(v);
    mapToLeafNode[graph_pointer] = leaf;
}

PEGraph * ART::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph * ART::retrieve(PEGraph_Pointer graph_pointer) {
    if (mapToLeafNode.find(graph_pointer)!= mapToLeafNode.end()){
        Node* node = mapToLeafNode[graph_pointer];
        vector<Edge> v = retrieveFromLeaf(node);
        return convertToPEGraph(v);
    }
    return nullptr;
}

//void ART::edgeSort(vector<vector<Edge *>> &graphs) {
//
//    for (auto &graph : graphs) {
//        for (auto &edge : graph) {
//            if (sortBase.find(*edge) == sortBase.end()) {
//                sortBase[*edge] = 1;
//            } else {
//                sortBase[*edge]++;
//            }
//        }
//    }
//
//    for (auto &graph : graphs) {
//        sort(graph.begin(), graph.end(), [=](const Edge *lhs, const Edge *rhs) -> bool {
//            return sortBase[*lhs] > sortBase[*rhs];
//        });
//    }
//}

void ART::edgeSort(vector<Edge> &vector_edge) {     //加入一个新的图之后,对内部进行重新排序

    for (auto &edge : vector_edge) {
        sortBase[edge]++;
    }

    sort(sortBase.begin(), sortBase.end(), [=](const Edge *lhs, const Edge *rhs) -> bool {
        return sortBase[*lhs] > sortBase[*rhs];
    });

}

void ART::addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    vector<Edge> edges = convertToVector(graph);     //##这里是不是返回引用更好一些
    // 对vector进行排序
    sort(edges.begin(), edges.end(), [=](const Edge *lhs, const Edge *rhs) -> bool {
        return sortBase[*lhs] > sortBase[*rhs];
    });

    Node *leaf = insert(edges);
    mapToLeafNode[pointer] = leaf;
    mapToLeafNum[leaf]++;
}

void ART::update_graphs(GraphStore *another) {
    update_graphs_sequential(another); // sequential
//    update_graphs_parallel(another); // in parallel
}

void ART::update_graphs_sequential(GraphStore *another) {
    ART* another_graphstore = dynamic_cast<ART*>(another);

    for(auto it: another_graphstore->mapToLeafNode){
        vector<Edge> vector = retrieveFromLeaf(it.second);
        PEGraph *pegraph = convertToPEGraph(vector);
        update(it.first, pegraph);
    }

}

void ART::clearEntryOnly() {

}

void ART::clear() {
//    postOrderDelete_iteration(root);          // two way to delete a tree
    _clear(root);
}

void ART::_clear(Node *node) {

    for(auto it: node->children)
        if(it->children.size())
            _clear(it);

    delete node;
}

void ART::postOrderDelete_iteration(Node *root) {
    if (root == nullptr) return;

    std::stack<Node *> stk;
    stk.push(root);
    while (!stk.empty()) {
        Node *top = stk.top();
        stk.pop();
        for (auto child:top->children) {
            stk.push(child);
        }
        // delete the node
        delete top;
    }
}

void ART::loadGraphStore(const string &file, const string &file_in) {
    this->deserialize(file);

    this->load_onebyone(file_in);
}

void ART::deserialize(const string &file) {
    if(readable){
        load_onebyone(file);
    }
    else{

    }
}

void ART::serialize(const string &file) {
    if(readable){
        ofstream myfile;
        myfile.open(file, std::ofstream::out);
        if (myfile.is_open()){
            for (auto& n : map) {
                //write a pegraph into file
                myfile << n.first << "\t";
                n.second->write_readable(myfile);
                myfile << "\n";
            }
            myfile.close();
        }
    }
    else{

    }
}

void ART::load_onebyone(const string &file) {
//for debugging
    Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);

    std::ifstream fin;
    fin.open(file);
    if(!fin) {
        cout << "can't load file_graphs: " << file << endl;
//	        exit (EXIT_FAILURE);
    }
    else{
        std::string line;
        while (getline(fin, line)) {
            if(line == ""){
                continue;
            }

            std::stringstream stream(line);
            std::string id;
            stream >> id;
            PEGraph_Pointer graph_pointer = atoi(id.c_str());
            PEGraph* pegraph = new PEGraph();
            pegraph->load_readable(stream);
            //since the file is appended, we just use the recent updated pegraph
            if (map.find(graph_pointer) != map.end()) {
                delete map[graph_pointer];
            }
            map[graph_pointer] = pegraph;
        }
        fin.close();

        //delete the old graphstore file
        FileUtil::delete_file(file);
    }

    //for debugging
    Logger::print_thread_info_locked("load-readable finished.\n", LEVEL_LOG_FUNCTION);

}


