//
// Created by w on 19-6-30.
//

#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
#define BACKEND1_FREQUENTITEMSETSTORE_H


#include <string>

static const string filePath = "../lib/file/intSets.txt";

static const int support = 2;

#include "graphstore.h"
#include "art.h"



class FrequentItemsetStore : public GraphStore {

public:

    FrequentItemsetStore();

    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) override;

    void update_graphs(GraphStore *another) override;

    void clearEntryOnly() override;

    void clear() override;

    string toString() override;

protected:
    void print(std::ostream &str) override;

    void toString_sub(std::ostringstream &strm) override;

private:

    FrequentItemsetStore(vector<unordered_set<int>> graphs);

    ~FrequentItemsetStore() override;

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

//    void loadGraphStore(const string &file) override;
    void loadGraphStore(const string& file, const string& folder_in) override;

private:
    std::unordered_map<PEGraph_Pointer, std::unordered_set<int>> pointerToEdgeIdSet;
    std::unordered_map<int, unordered_set<int>> intToIdFrequentItemset;
    std::unordered_map<int, Edge> idToEdge;
    std::unordered_map<Edge, int> edgeToId;
    int num_frequentItemset;
    int edgeId;
    int num_pointer;

    PEGraph *convertToPeGraph(unordered_set<int> edgeSet);

    unordered_set<int> convertToEdgeSet(PEGraph *peGraph);

    void retrieveSet(unordered_set<int> &graphSet, unordered_set<int> &realEdgeSet);

    unordered_set<int> frequentItemsetMining_closed(vector<unordered_set<int>> &graphs);

    unordered_set<int> frequentItemsetMining_minimum(vector<unordered_set<int>> &graphs);


    void writeToFile(vector<unordered_set<int>> &graphs);

    unordered_set<int> readFromFile();


    void deserialize(const string &file);

    void load_onebyone(const string &file);
};


#endif //BACKEND1_FREQUENTITEMSETSTORE_H
