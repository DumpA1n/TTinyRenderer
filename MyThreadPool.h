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
    auto enqueue(F&& f, Args&&... args) -> std::future<std::result_of<F(Args...)>::type> {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.push(std::forward(f));
        }
        condition.notify_one();
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
                    if (terminate || !tasks.empty())
                        return;
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