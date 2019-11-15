#include <iostream>
#include <thread>
#include <string>
#include <ctime>
#include <mutex>
#include <chrono>
#include "threadpool.h"

#define CYCLE 50
/*used function*/
/*taskid and stra shoud be the input data, strb is the output place holder*/
void function1(int taskid, const std::string& stra, std::string& strb) {
    std::cout << "task:" << taskid << "---this thread ID:" << std::this_thread::get_id() << std::endl;
    strb = stra + " and this is string b modified";
    auto f = 1000000000;
    while (f > 1){
        f /= 1.00001;
        auto j = 100;
        while (j > 0){
            j -= 1;
        }
    }
}

int main() {
    //main thread number
    std::cout << "main thread ID:" << std::this_thread::get_id() << std::endl;
    //max number of threads
    std::cout << "number of cores/threads:" << std::thread::hardware_concurrency() << std::endl;
    //create thread pool
    std::cout << "---------------------stating thread pool-------------------" << std::endl;
    ThreadPool pool(6);

    std::string strb[CYCLE]; //output holder holder

    std::cout << "-----------------first multi thread section----------------" << std::endl;
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    for (int i = 0; i < CYCLE; ++i)
    {
        //prepare data
        std::string stra = "Constant Data to thread: " + std::to_string(i);

        //creat packaged task
        std::packaged_task<void()> ta(std::bind(function1, i, std::move(stra), std::ref(strb[i])));
        //push the packaged_task to task queue
        pool.enqueue(std::move(ta));
    }
    while(!pool.queueEmpty()) {;}
    clock_gettime(CLOCK_MONOTONIC, &t2);
    double time_diff_sec = (t2.tv_sec-t1.tv_sec) + (t2.tv_nsec-t1.tv_nsec)/1000000000.0;
    std::cout << "Total Time multi thread: " << time_diff_sec << "s" << std::endl;

    //verify the result
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    for (int i = 0; i < CYCLE; ++i)
    {
        std::cout << "from main: " << strb[i] << std::endl;
    }


    std::cout << "-----------------single thread section----------------" << std::endl;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < CYCLE; ++i)
    {
        //prepare data
        std::string stra = "Constant Data to main: " + std::to_string(i);

        //call function1
        function1(i,stra,strb[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time_diff_sec = (t2.tv_sec-t1.tv_sec) + (t2.tv_nsec-t1.tv_nsec)/1000000000.0;
    std::cout << "Total Time single thread: " << time_diff_sec << "s" << std::endl;
    //verify the result
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    for (int i = 0; i < CYCLE; ++i)
    {
        std::cout << "from main: " << strb[i] << std::endl;
    }

    std::cout << "-----------------second multi thread section----------------" << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (int i = 0; i < CYCLE; ++i)
    {
        //prepare data
        std::string stra = "Constant Data to thread: " + std::to_string(i);

        //creat packaged task
        std::packaged_task<void()> ta(std::bind(function1, i, std::move(stra), std::ref(strb[i])));
        //push the packaged_task to task queue
        pool.enqueue(std::move(ta));
    }
    while(!pool.queueEmpty()) {;}
    clock_gettime(CLOCK_MONOTONIC, &t2);
    time_diff_sec = (t2.tv_sec-t1.tv_sec) + (t2.tv_nsec-t1.tv_nsec)/1000000000.0;
    std::cout << "Total Time multi thread: " << time_diff_sec << "s" << std::endl;

    //verify the result
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    for (int i = 0; i < CYCLE; ++i)
    {
        std::cout << "from main: " << strb[i] << std::endl;
    }
    return 0;
}
