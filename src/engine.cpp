/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include <iostream>
#include "comp/cfg_compute_asyn.h"


static const char *const FINAL_FILE = "/home/zqzuo/Desktop/inlined/final";
static const char *const STMT_INFO_FILE = "/home/zqzuo/Desktop/inlined/id_stmt_info.txt";
static const char *const SINGLETON_FILE = "/home/zqzuo/Desktop/inlined/var_singleton_info.txt";
static const char *const POINTTO_FILE = "/home/zqzuo/Desktop/inlined/rules_pointsto.txt";
using namespace std;



int main() {
    CFG *cfg = new CFG_map();
    GraphStore *graphstore = new NaiveGraphStore();
    Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();

//	Partition *partition = nullptr;
//	cout << cfg << endl;

    CFGCompute::load(FINAL_FILE, STMT_INFO_FILE, cfg,
                     SINGLETON_FILE, singletons, graphstore, POINTTO_FILE, grammar);
    CFGCompute::do_worklist_synchronous(cfg, graphstore, grammar, singletons);
//	CFGCompute_asyn::do_worklist_asynchronous(cfg, graphstore, grammar, singletons);

    delete cfg;
    delete graphstore;
    delete grammar;
    //##为什么没有delete singleton？
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
}
