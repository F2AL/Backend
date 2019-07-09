//
// Created by w on 19-6-30.
//

#ifndef BACKEND1_FREQUENTITEMSETSTORE_H
#define BACKEND1_FREQUENTITEMSETSTORE_H


static const char *const FILEPATH = "../lib/file/intSets.txt";

static const int SUPPORT = 2;

static const char *const COMMOND_CLOSED_ALP = "../lib/eclat -tc ../lib/file/test_a ../lib/file/out_CLOSED";     // 确定这是closed的吗，感觉输出结果有点问题
static const char *const COMMOND_CLOSED_NUM = "../lib/eclat -tc ../lib/file/test ../lib/file/out_";     // todo 以下三个位置初的num都需要补全

static const char *const COMMOND_MAX_ALP = "../lib/eclat -tm ../lib/file/test_a ../lib/file/out_MAX ";      // ##触及到自己的知识盲区了
static const char *const COMMOND_MAX_NUM = "../lib/eclat -tm ../lib/file/test ../lib/file/out_";

static const char *const COMMOND_FREQUENT_ALP = "../lib/eclat -ts ../lib/file/test ../lib/file/out_FREQUENT";
static const char *const COMMOND_FREQUENT_NUM = "../lib/eclat -ts ../lib/file/test ../lib/file/out_";

static const char *const COMMOND_FREE_ALP = "../lib/eclat -tg ../lib/file/test ../lib/file/out_FREE";
static const char *const COMMOND_FREE_NUM = "../lib/eclat -tg ../lib/file/test ../lib/file/out_";


static const int SELECTED_NUM = 2;

#include "graphstore.h"
#include "art.h"



class FrequentItemsetStore : public GraphStore {

public:

    FrequentItemsetStore();

    FrequentItemsetStore(int frequentItemsetNum, int edgeId, int numP);

private:

    explicit FrequentItemsetStore(vector<set<int>> graphs);

    ~FrequentItemsetStore() override;

    PEGraph *retrieve(PEGraph_Pointer graph_pointer) override;

    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) override;

    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) override;

    void loadGraphStore(const string &file) override;

private:
    std::unordered_map<PEGraph_Pointer, std::set<int>> intToEdgeSet;
    std::unordered_map<int, set<int>> intToFrequentItemset;
    std::unordered_map<int, Edge> intToEdge;
    std::unordered_map<Edge, int> edgeToInt;
    int frequentItemsetNum;
    int edgeId;
    int num_p;

    PEGraph *convertToPeGraph(set<int> edgeSet);

    set<int> convertToEdgeSet(PEGraph *peGraph);

    void retrieveSet(set<int> &graphSet, set<int> &realEdgeSet);

    vector<set<int>> frequentItemsetMining_closed(int min_support, vector<set<int>> &graphs);

    vector<set<int>> frequentItemsetMining_closed_once(int min_support, vector<set<int>> &graphs);

    vector<set<int>> frequentItemsetMining_minimum(int min_support, vector<set<int>> &graphs);


    void writeToFile(vector<set<int>> &graphs);

    vector<set<int>> readFromFile();


    void handle(set<int> &edgeSet);

    vector<set<int>> frequentItemsets;
};


#endif //BACKEND1_FREQUENTITEMSETSTORE_H
