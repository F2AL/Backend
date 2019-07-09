/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include <iostream>
#include <FrequentItemsetStore.h>
#include "comp/cfg_compute_asyn.h"


static const char *const FINAL_FILE = "/home/w/CLionProjects/backend-nju/lib/graphFile/final";      // todo 修改这里的文件路径
static const char *const STMT_INFO_FILE = "/home/w/CLionProjects/backend-nju/lib/graphFile/id_stmt_info.txt";
static const char *const SINGLETON_FILE = "/home/w/CLionProjects/backend-nju/lib/graphFile/var_singleton_info.txt";
static const char *const POINTTO_FILE = "/home/w/CLionProjects/backend-nju/lib/graphFile/index_var_info.txt";           //todo 是不是有什么问题呢
using namespace std;



int main() {
    system(COMMOND_MAX_ALP);
    system(COMMOND_CLOSED_ALP);
    system(COMMOND_FREQUENT_ALP);
    cout<<123<<endl;
	return 0;
}
