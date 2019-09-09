//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

void FrequentItemsetStore::loadGraphStore(const string& file, const string& file_in) {
    this->deserialize(file);


    this->load_onebyone(file_in);
}


FrequentItemsetStore::~FrequentItemsetStore() {

}

PEGraph *FrequentItemsetStore::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph *FrequentItemsetStore::retrieve(PEGraph_Pointer graph_pointer) {
//    retrieve the pegraph set
    std::unordered_set<int> edgeSet = pointerToEdgeIdSet[graph_pointer];

    if(edgeSet.empty())
        return nullptr;

//    convert it to PEGraph*
    PEGraph *peGraph = convertToPeGraph(edgeSet);
    return peGraph;
}

void FrequentItemsetStore::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {

    // todo delete the pointer  ##
    std::unordered_set<int> edgeSet = convertToEdgeSet(pegraph);
    pointerToEdgeIdSet[graph_pointer] = edgeSet;
}

void FrequentItemsetStore::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, pegraph);
}

// convert from edgeSet to peGraph*
PEGraph *FrequentItemsetStore::convertToPeGraph(unordered_set<int> edgeSet) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;
    unordered_set<int> realGraphSet;
    retrieveSet(edgeSet, realGraphSet);

    for (auto edgeId : edgeSet) {
        vertexid_t src = idToEdge[edgeId].src;
        vertexid_t dst = idToEdge[edgeId].des;
        label_t label = idToEdge[edgeId].label;
        if (graph.find(src) == graph.end()) {
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }
    peGraph->setGraph(graph);
    return peGraph;
}

unordered_set<int> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
    unordered_set<int> edgeSet;
    for (auto &it : peGraph->getGraph()) {

        int size = it.second.getSize();
        vertexid_t *edges = it.second.getEdges();
        label_t *labels = it.second.getLabels();
        Edge edge;
        for (int i = 0; i < size; ++i) {
            edge = Edge(it.first, edges[i], labels[i]);
            if (edgeToId.find(edge) == edgeToId.end()) {
                edgeToId[edge] = edgeId;
                idToEdge[edgeId++] = edge;
            }
            edgeSet.insert(edgeToId[edge]);
        }
    }

    return edgeSet;
}

void FrequentItemsetStore::retrieveSet(unordered_set<int> &graphSet, unordered_set<int> &realEdgeSet) {

    for (int it : graphSet) {
        if (it >= 0) realEdgeSet.insert(it);
        else retrieveSet(intToIdFrequentItemset[it], realEdgeSet);
    }
}

/*
 * 实现思路:
 * 1. 将信息存储到文件中
 * 2. 对文件调用 eclat 命令
 * 3. 将结果的前10行读取进内存中
 * 4. 对可以进行替换的内容进行替换,并将新的频繁项集记录下来
 * 5.
 * */
unordered_set<int> FrequentItemsetStore::frequentItemsetMining_closed(vector<unordered_set<int>> &graphs) {
    writeToFile(graphs);

    system("../lib/eclat -tc ../lib/file/test ../lib/file/out");            // should we stop for a while?

    unordered_set<int> v = readFromFile();            // only read the first line
    return v;
}

unordered_set<int> FrequentItemsetStore::frequentItemsetMining_minimum(vector<unordered_set<int>> &graphs) {
    writeToFile(graphs);

    system("../lib/eclat -tm ../lib/file/test ../lib/file/out");            // should we stop for a while?

    unordered_set<int> v = readFromFile();            // only read the first line
    return v;
}


FrequentItemsetStore::FrequentItemsetStore() : num_frequentItemset(0), edgeId(0) {}

FrequentItemsetStore::FrequentItemsetStore(vector<unordered_set<int>> graphs) {
    unordered_set<int> frequentItemset = frequentItemsetMining_closed(graphs);       //compute the frequent itemset
    while (!frequentItemset.empty()) {
        for (auto &graph: graphs) {
            bool flag = true;           //check if the graphSet need to update
            for (auto &elem : frequentItemset) {
                if (!graph.count(elem)) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
//              delete the element in FISet
                for (auto &elem : frequentItemset) {
                    graph.erase(graph.find(elem));
                }
//              insert a special Edge for the num_pointer
//                Edge newEdge = Edge(-num_pointer, 0, 0);
                int newEdge = -num_pointer;
                num_pointer++;
                intToIdFrequentItemset[newEdge] = frequentItemset;
                graph.insert(newEdge);
            }
        }
        frequentItemset = frequentItemsetMining_closed(graphs);
    }
}

///
/// \param graphs 将文件写入 filePath 中去
void FrequentItemsetStore::writeToFile(vector<unordered_set<int>> &graphs) {
    ofstream output;
    output.open(filePath);

    for (const auto& graph : graphs) {
        for (auto _edgeId: graph) {
            cout << _edgeId << " ";
        }
        cout << endl;
    }
    output.close();
}

unordered_set<int> FrequentItemsetStore::readFromFile() {
    vector<unordered_set<int>> graphs;

    ifstream finput;
    finput.open(filePath);
    if (!finput) {
        cout << "can't load file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(finput, line) && line != "") {
        unordered_set<int> graph;
        std::stringstream stream(line);
        int id;
        while (stream >> id) graph.insert(id);
        graphs.push_back(graph);
    }
    return graphs;
    finput.close();
}

void FrequentItemsetStore::addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) {

}

void FrequentItemsetStore::update_graphs(GraphStore *another) {

}

void FrequentItemsetStore::clearEntryOnly() {

}

void FrequentItemsetStore::clear() {

}

string FrequentItemsetStore::toString() {
    return GraphStore::toString();
}

void FrequentItemsetStore::print(std::ostream &str) {

}

void FrequentItemsetStore::toString_sub(std::ostringstream &strm) {

}

void FrequentItemsetStore::deserialize(const string &file) {

}

void FrequentItemsetStore::load_onebyone(const string &file) {

}



