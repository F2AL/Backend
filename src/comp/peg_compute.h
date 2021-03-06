#ifndef COMP_PEGCOMPUTE_H
#define COMP_PEGCOMPUTE_H
#include "computationset.h"
#include "containerstomerge.h"
#include "array/arraystomerge.h"
#include "../common/CommonLibs.hpp"
#include "../preproc/grammar.h"
#include "myalgorithm.h"
//#include "../utility/Logger.hpp"
#include "computationset.h"

class PEGCompute {
private:

    static void genS_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_delta(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void genD_RuleEdges_old(vertexid_t index, ComputationSet *compset,ContainersToMerge &containers,Grammar *grammar);
    static void mergeToDeletedGraph(vertexid_t i_new, std::unordered_map<vertexid_t, EdgeArray>* m, ComputationSet* compset);
    static void getEdgesToMerge(vertexid_t index, ComputationSet *compset,bool oldEmpty,bool deltaEmpty,ContainersToMerge &containers,Grammar *grammar);

public:
    static long startCompute_add(ComputationSet *compset,Grammar *grammar);
    static long startCompute_delete(ComputationSet *compset, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray> *m);

    static long computeOneVertex(vertexid_t index, ComputationSet *compset, Grammar *grammar);

    static void computeOneIteration(ComputationSet *compset,Grammar *grammar);
    static void postProcessOneIteration(ComputationSet *compset, bool isDelete, std::unordered_map<vertexid_t, EdgeArray> *m = nullptr);

};
#endif
