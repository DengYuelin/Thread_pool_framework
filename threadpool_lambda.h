//
// Created by yuelin on 11/15/19.
//

#ifndef MULTITHREAD_THREADPOOL_LAMBDA_H
#define MULTITHREAD_THREADPOOL_LAMBDA_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include <queue>
/*This thread pool takes lambdas which have no parameter as input*/
class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(std::size_t numThreads)
    {
        start(numThreads);
    }

    ~ThreadPool()
    {
        stop();
    }

//    template<class T>
//    auto enqueue(T task)->std::future<decltype(task())>
//    {
//        auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
//
//        {
//            std::unique_lock<std::mutex> lock{mEventMutex};
//            mTasks.emplace([=] {
//                (*wrapper)();
//            });
//        }
//
//        mEventVar.notify_one();
//        return wrapper->get_future();
//    }

    void enqueue(Task task)
    {
        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mTasks.emplace(std::move(task));
        }
        mEventVar.notify_one();
    }

    bool queueEmpty()
    {
        return mTasks.empty();
    }

private:
    std::vector<std::thread> mThreads;

    std::condition_variable mEventVar;

    std::mutex mEventMutex;
    bool mStopping = false;

    std::queue<Task> mTasks;

    void start(std::size_t numThreads)
    {
        for (auto i = 0u; i < numThreads; ++i)
        {
            mThreads.emplace_back([=] {
                while (true)
                {
                    Task task;

                    {
                        std::unique_lock<std::mutex> lock{mEventMutex};

                        mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

                        if (mStopping && mTasks.empty())
                        {
                            std::cout << "thread breaking" << std::endl;
                            break;
                        }
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }

                    task();
                }
            });
        }
    }

    void stop() noexcept
    {
        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mStopping = true;
        }

        mEventVar.notify_all();

        for (auto &thread : mThreads) {
            thread.join();
            std::cout << "thread killed" << std::endl;
        }
    }
};

#endif //MULTITHREAD_THREADPOOL_LAMBDA_H
