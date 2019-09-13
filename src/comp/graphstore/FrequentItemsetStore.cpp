//
// Created by w on 19-6-30.
//

#include "FrequentItemsetStore.h"

void FrequentItemsetStore::loadGraphStore(const string &file, const string &file_in) {
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

    if (edgeSet.empty())
        return nullptr;

//    convert it to PEGraph*        todo 转换的时间代价是不是也需要考虑
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
PEGraph *FrequentItemsetStore::convertToPeGraph(unordered_set<int> &edgeSet) {
    PEGraph *peGraph = new PEGraph;
    std::unordered_map<vertexid_t, EdgeArray> graph;
    unordered_set<int> realGraphSet;
    retrieveSet(edgeSet, realGraphSet);

    for (auto _edgeId : realGraphSet) {
        vertexid_t src = idToEdge[_edgeId].src;
        vertexid_t dst = idToEdge[_edgeId].des;
        label_t label = idToEdge[_edgeId].label;
        const auto &not_found = graph.end();
        if (graph.find(src) == not_found) {
            graph[src] = EdgeArray();
        }
        graph[src].addOneEdge(dst, label);
    }
    peGraph->setGraph(graph);
    return peGraph;
}

unordered_set<int> FrequentItemsetStore::convertToEdgeSet(PEGraph *peGraph) {
    unordered_set<int> edgeSet;
    unordered_map<vertexid_t, EdgeArray> &graph = peGraph->getGraph();
    for (auto &it : graph) {

        int size = it.second.getSize();
        vertexid_t *edges = it.second.getEdges();
        label_t *labels = it.second.getLabels();
        Edge edge;
        for (int i = 0; i < size; ++i) {
            edge = Edge(it.first, edges[i], labels[i]);
            const auto &not_found = edgeToId.end();
            if (edgeToId.find(edge) == not_found) {
                edgeToId[edge] = edgeId;            //edge从哪里开始增加
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
std::vector<std::unordered_set<int>>
FrequentItemsetStore::frequentItemsetMining_closed(vector<unordered_set<int>> &graphs) {
    writeToFile(graphs);

    system("../lib/eclat -tc ../lib/file/test ../lib/file/out");            // should we stop for a while?

    std::vector<std::unordered_set<int>> v = readFromFile();            // only read the first line
    return v;
}

std::vector<std::unordered_set<int>>
FrequentItemsetStore::frequentItemsetMining_minimum(vector<unordered_set<int>> &graphs) {
    writeToFile(graphs);

    system("../lib/eclat -tm ../lib/file/test ../lib/file/out");            // should we stop for a while?

    std::vector<std::unordered_set<int>> v = readFromFile();            // only read the first line
    return v;
}


FrequentItemsetStore::FrequentItemsetStore() : edgeId(0), num_itemset(0) {}

/// 使用itemset替换掉 edgeset 中的元素
/// \param edgeset
/// \param itemset
void FrequentItemsetStore::change(unordered_set<int> &edgeset, unordered_set<int> &itemset) {
    //删除 itemset中的元素
    for (auto &item : itemset) {
        edgeset.erase(item);
    }
    num_itemset++;
    int newEdge = -num_itemset;
    edgeset.insert(newEdge);
//    intToIdFrequentItemset[newEdge] = itemset;
    intToEdgeIdSet[newEdge] = itemset;
}

FrequentItemsetStore::FrequentItemsetStore(vector<unordered_set<int>> graphs) {
    std::vector<std::unordered_set<int>> vector_itemset = frequentItemsetMining_closed(graphs);
    //compute the frequent itemset
    for (auto &graph: graphs) {
        bool flag = true;           //check if the graphSet need to update
        auto const not_found = graph.end();
        for (auto &itemset : vector_itemset) {
//            graph.count(elem)
            for (auto &item : itemset) {
                if (graph.find(item) == not_found) {          //如果发现有itemset中的元素不存在于graph,不需要进行更新
                    flag = false;
                    break;
                }
            }
            if (flag) {               //如果需要更新,则进行更新操作
                change(graph, itemset);
            }
        }
    }
}

///
/// \param graphs 将文件写入 filePath 中去
void FrequentItemsetStore::writeToFile(vector<unordered_set<int>> &graphs) {
    ofstream output;
    output.open(filePath);

    for (const auto &graph : graphs) {
        for (auto _edgeId: graph) {
            cout << _edgeId << " ";
        }
        cout << endl;
    }
    output.close();
}

vector<unordered_set<int>> FrequentItemsetStore::readFromFile() {
    vector<unordered_set<int>> vector_itemSet;          //这是一个局部变量,所以不能返回引用

    ifstream finput;
    finput.open(filePath);
    if (!finput) {
        cout << "can't load file: " << filePath << endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(finput, line) && !line.empty()) {
        unordered_set<int> itemset;
        std::stringstream stream(line);
        int id;
        while (stream >> id) itemset.insert(id);
        vector_itemSet.push_back(itemset);
    }
    finput.close();
    return vector_itemSet;
}

void FrequentItemsetStore::addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) {
    std::lock_guard<std::mutex> lockGuard(mutex);
    update(pointer, graph);
}

void FrequentItemsetStore::update_graphs(GraphStore *another) {
    update_graphs_sequential(another); // sequential
    update_graphs_parallel(another); // in parallel
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

void FrequentItemsetStore::update_graphs_sequential(GraphStore *another) {

}

void FrequentItemsetStore::update_graphs_parallel(GraphStore *another) {

}





