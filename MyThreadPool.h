#pragma once

#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>

class MyThreadPool {
public:
    MyThreadPool(int capacity) : terminate(false) {
        for (int i = capacity; i--;) {
            threads.emplace_back([this]() { thread_loop(); });
        }
    }
    ~MyThreadPool() {
        terminate = true;
        condition.notify_all();
        for (auto& t : threads) {
            if (t.joinable())
                t.join();
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

private:
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> terminate;

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    void thread_loop() {
        while (!terminate) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                condition.wait(lock, [this]() {
                    return terminate || !tasks.empty();
                });

                if (terminate && tasks.empty())
                    return;

                task = tasks.front();
                tasks.pop();
            }
            task();
        }
    }
};