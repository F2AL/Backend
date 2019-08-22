//
// Created by dell on 8/7/19.
//

#ifndef BACKEND_MYTIMER_H
#define BACKEND_MYTIMER_H


class myTimer {
private:


public:

    static int count_combine_synchronous;
    static float duration_combine_synchronous;

    static int count_transfer;
    static float duration_transfer;

    static int count_retrieve;
    static float duration_retrieve;

    static int count_update;
    static float duration_update;


    static int count_peg_compute_add;
    static float duration_peg_compute_add;

    static int count_peg_compute_delete;
    static float duration_peg_compute_delete;

    static int count_startCompute_delete;
    static float duration_startCompute_delete;

    static int count_startCompute_add;
    static float duration_startCompute_add;

    static int count_postProcessOneIteration_add;
    static float duration_postProcessOneIteration_add;

    static int count_postProcessOneIteration_delete;
    static float duration_postProcessOneIteration_delete;

    static int count_computeOneIteration_add;
    static float duration_computeOneIteration_add;

    static int count_computeOneIteration_delete;
    static float duration_computeOneIteration_delete;


    static void addCountCombineSynchronous();

    static void addDurationCombineSynchronous(double duration);

    static void addCountTransfer();

    static void addDurationTransfer(double duration);

    static void addCountPegComputeAdd();

    static void addDurationPegComputeAdd(double duration);

    static void addCountPegComputeDeleted();

    static void addDurationPegComputeDeleted(double duration);


    static void addDurationRetrieve(double duration);

    static void addCountRetrieve();

    static void addDurationUpdate(double duration);

    static void addCountUpdate();

    static void add_duration_startCompute_add(double duration);

    static void add_count_startCompute_add();

    static void add_duration_startCompute_delete(double duration);

    static void add_count_startCompute_delete();


};


#endif //BACKEND_MYTIMER_H
