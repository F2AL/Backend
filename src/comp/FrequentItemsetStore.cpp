//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

void FrequentItemsetStore::loadGraphStore(const string &file_singleton) {

}


FrequentItemsetStore::~FrequentItemsetStore() {

}

PEGraph *FrequentItemsetStore::retrieve_locked(PEGraph_Pointer graph_pointer) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    return retrieve(graph_pointer);
}

PEGraph *FrequentItemsetStore::retrieve(PEGraph_Pointer graph_pointer) {
//    retrieve the pegraph set
    std::set<int> edgeSet = intToEdgeSet[graph_pointer];
//    convert it to PEGraph*
    PEGraph *peGraph = convertToPeGraph(edgeSet);
    return nullptr;

//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        return new PEGraph(mapToPEG[graph_pointer]);
//    }
//    mapToPEG[graph_pointer] = new PEGraph();
//    return new PEGraph();
}

void FrequentItemsetStore::update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    if (mapToPEG.find(graph_pointer) != mapToPEG.end()) {
//        delete mapToPEG[graph_pointer];
//    }
//    mapToPEG[graph_pointer] = new PEGraph(pegraph);

//    std::set<Edge>* graphSet = mapToEdgesSet[graph_pointer];

    std::set<int> edgeSet = convertToEdgeSet(pegraph);
    handle(edgeSet);
    intToEdgeSet[graph_pointer] = edgeSet;
}

void FrequentItemsetStore::update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(graph_pointer, pegraph);
}

// convert from edgeSet to peGraph*
PEGraph *FrequentItemsetStore::convertToPeGraph(set<int> edgeSet) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;
    set<int> realGraphSet;
    retrieveSet(edgeSet, realGraphSet);

    for (auto edgeId : edgeSet) {
        vertexid_t src = intToEdge[edgeId].src;
        vertexid_t dst = intToEdge[edgeId].des;
        label_t label = intToEdge[edgeId].label;
        if (graph.find(src) == graph.end()) {
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }
    peGraph->setGraph(graph);
    return peGraph;
}

set<int> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
    set<int> edgeSet;
    for (auto &it : peGraph->getGraph()) {

        int size = it.second.getSize();
        vertexid_t *edges = it.second.getEdges();
        label_t *labels = it.second.getLabels();
        Edge edge;
        for (int i = 0; i < size; ++i) {
            edge = Edge(it.first, edges[i], labels[i]);
            if (edgeToInt.find(edge) == edgeToInt.end()) {
                edgeToInt[edge] = edgeId;
                intToEdge[edgeId++] = edge;
            }
            edgeSet.insert(edgeToInt[edge]);
        }
    }
    return edgeSet;
}

void FrequentItemsetStore::retrieveSet(set<int> &graphSet, set<int> &realEdgeSet) {

    for (int it : graphSet) {
        if (it >= 0) realEdgeSet.insert(it);
        else retrieveSet(intToFrequentItemset[it], realEdgeSet);
    }
}

//
vector<set<int>> FrequentItemsetStore::frequentItemsetMining_closed(int min_support, vector<set<int>> &graphs) {
    writeToFile(graphs);

    system(COMMOND_CLOSED_ALP);

    vector<set<int>> v = readFromFile();            // only read the first line
    return v;
}

// 只做一次挖掘，从中取出符合条件的itemset， 需要传入一个参数，作为选择的set的数目
vector<set<int>> FrequentItemsetStore::frequentItemsetMining_closed_once(int min_support, vector<set<int>> &graphs) {
    writeToFile(graphs);

    system(COMMOND_CLOSED_ALP);

    vector<set<int>> v = readFromFile();            // only read the first line

    //对v进行处理
    // todo 对结果根据依据进行排序


    // 截取前 SELECTED_NUM 个位置
    auto first = v.begin();
    auto last  = v.begin() + SELECTED_NUM;
    vector<set<int>> result(first, last);

    return result;
}

vector<set<int>> FrequentItemsetStore::frequentItemsetMining_minimum(int min_support, vector<set<int>> &graphs) {
    writeToFile(graphs);

    system(COMMOND_MAX_ALP);

    vector<set<int>> v = readFromFile();            // only read the first line

    //todo filter from the origin data

    return v;
}


// 这是只计算一轮的情形
FrequentItemsetStore::FrequentItemsetStore(vector<set<int>> graphs) {
    frequentItemsets = frequentItemsetMining_closed_once(SUPPORT, graphs);       //compute the frequent itemset

    /*策略：
     * 需要n*m轮遍历 其中 n代表频繁项集的数目， m代表vector中set的数目 （todo 为了优化的目的，记录n中每个数据的supp， 内层遍历处理完supp个数据后就break 其适用条件可能与挖掘的策略有关）
     * 1. 循环对n 和 m进行遍历
     * 2. 发现T可以替换掉set，就用T替换掉set，并在map中添加一条记录
     *  todo 从实现的角度，这是一个三重循环，复杂度有点高
     * */
    for(const auto& frequentItemset: frequentItemsets){
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
                //              insert a special Edge for the num_p
                //                Edge newEdge = Edge(-num_p, 0, 0);
                int newEdge = -num_p;
                num_p++;
                intToFrequentItemset[newEdge] = frequentItemset;
                graph.insert(newEdge);
            }
        }
    }

//    for (const auto& frequentItemset_selected: frequentItemsets) {
//        while (!frequentItemset_selected.empty()) {
//            for (auto &graph: graphs) {
//                bool flag = true;           //check if the graphSet need to update
//                for (auto &elem : frequentItemset_selected) {
//                    if (!graph.count(elem)) {
//                        flag = false;
//                        break;
//                    }
//                }
//                if (flag) {
//                    //              delete the element in FISet
//                    for (auto &elem : frequentItemset_selected) {
//                        graph.erase(graph.find(elem));
//                    }
//                    //              insert a special Edge for the num_p
//                    //                Edge newEdge = Edge(-num_p, 0, 0);
//                    int newEdge = -num_p;
//                    num_p++;
//                    intToFrequentItemset[newEdge] = frequentItemset_selected;
//                    graph.insert(newEdge);
//                }
//            }
////            frequentItemset_selected = frequentItemsetMining_closed(SUPPORT, graphs);  // todo how to handle this?
//        }
//    }
}

void FrequentItemsetStore::writeToFile(vector<set<int>> &graphs) {
    ofstream output;
    output.open(FILEPATH);

    for (const auto& graph : graphs) {
        for (auto edgeId: graph) {
            cout << edgeId << " ";            // todo check the end of the line
        }
        cout << endl;
    }
    output.close();
}

vector<set<int>> FrequentItemsetStore::readFromFile() {
    vector<set<int>> graphs;
    ifstream fin;
    fin.open(FILEPATH);
    if (!fin) {
        cout << "can't load file: " << FILEPATH << endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(fin, line) && line != "") {
        set<int> graph;
        std::stringstream stream(line);
        int id;
        while (stream >> id) graph.insert(id);
        graphs.push_back(graph);
    }
    fin.close();
    return graphs;
}

FrequentItemsetStore::FrequentItemsetStore(int frequentItemsetNum, int edgeId, int numP) : frequentItemsetNum(
        frequentItemsetNum), edgeId(edgeId), num_p(numP) {}

void FrequentItemsetStore::handle(set<int> &edgeSet) {

    for (auto map : intToFrequentItemset){
        bool flag = true;           //check if the graphSet need to update
        for(auto &elem : map.second){
            if (!edgeSet.count(elem)) {
                flag = false;
                break;
            }
        }
        if (flag) {
            //              delete the element in FISet
            for (auto &elem : map.second) {
                edgeSet.erase(edgeSet.find(elem));
            }
            edgeSet.insert(map.first);
        }
    }


}





