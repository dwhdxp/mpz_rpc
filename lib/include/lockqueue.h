#pragma once
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

// 日志队列，生产者消费者模型
template <typename T>
class LockQueue {
public:
    // producer：只有一个日志线程负责写日志
    void Push(const T& data) {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_queue.push(data);
        m_cv.notify_one();
    }

    // consumer
    T Pop() {
        std::unique_lock<std::mutex> lock(m_mtx);
        // 队列为空时被阻塞，队列不为空时才继续执行
        m_cv.wait(lock, [&]() { return !m_queue.empty(); });
        T data = m_queue.front();
        m_queue.pop();
        return data;
    }

private:
    std::queue<T> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};
