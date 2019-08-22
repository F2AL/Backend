//
// Created by dell on 8/7/19.
//

#include "myTimer.h"


void myTimer::addCountCombineSynchronous() {
    count_combine_synchronous++;
}

void myTimer::addCountTransfer() {
    count_transfer++;
}

void myTimer::addDurationCombineSynchronous(double duration) {
    duration_combine_synchronous += duration;
}

void myTimer::addDurationTransfer(double duration) {
    duration_transfer += duration;
}

void myTimer::addDurationRetrieve(double duration) {
    duration_retrieve += duration;
}

void myTimer::addCountRetrieve() {
    count_retrieve++;
}

void myTimer::addDurationUpdate(double duration) {
    duration_update += duration;
}

void myTimer::addCountUpdate() {
    count_update++;
}

void myTimer::addCountPegComputeAdd() {
    count_peg_compute_add++;
}

void myTimer::addDurationPegComputeAdd(double duration) {
    duration_peg_compute_add += duration;
}

void myTimer::addCountPegComputeDeleted() {
    count_peg_compute_delete++;
}

void myTimer::addDurationPegComputeDeleted(double duration) {
    duration_peg_compute_delete += duration;
}

void myTimer::add_duration_startCompute_add(double duration) {
    duration_startCompute_add += duration;
}

void myTimer::add_count_startCompute_add() {
    count_startCompute_add++;
}

void myTimer::add_duration_startCompute_delete(double duration) {
    duration_startCompute_delete+=duration;
}

void myTimer::add_count_startCompute_delete() {
    count_startCompute_delete++;
}
