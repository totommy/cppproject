#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <utility> // std::move, std::forward

using namespace std;

// 線程池實作 (c++11 語法大全)
// 線程池是符合 生產者 - 消費者模型 所以要學會如何封裝
// 手搓線程池
class ThreadPool {
private:
    vector<thread> threads;
    queue<function<void()>> taskQueue;

    mutex mtx;
    bool isStop;
    condition_variable cv;
public:
    ThreadPool(int numThreads) : isStop(false) {
        for (int i = 1; i <= numThreads; i++) {
            threads.emplace_back([this]() {
                // 消費者模型
                while (true) {
                    {
                        unique_lock<mutex> lk(mtx);

                        cv.wait(lk, [this]() { return !taskQueue.empty() || isStop; });
                        // 要阻塞多線程來避免 CPU busy waiting 
                        // 任務隊列為非空 或是 停止了放任務(有可能放了任務後, 延遲了一下才繼續放任務) 
                        // 就要 讓程式往下走
                        // 任務隊列為空 或是 未停止了放任務
                        // 就要 讓多線程睡覺等待
                        if (taskQueue.empty() && isStop) return;
                        // 任務隊列為空 且 都不放任務了 就直接 return;
                        // 這裡也要判斷 任務隊列為空 因為有可能還在執行任務時(也就是隊列任務非空時)
                        // 我們不希望就終止所有線程
                        function<void()> task = taskQueue.front();
                        taskQueue.pop();
                        // do task
                        task();
                    }
                    
                }
            });
        }
    }

    ~ThreadPool() {
        // 解構函數
        // 意思就是物件要銷毀了, 也就是說不會想要再加任務到任務隊列中了
        {
            unique_lock<mutex> lk(mtx);
            isStop = true;
        }
        cv.notify_all(); // 叫所有線程趕快運行完成
        for (auto& t : threads) t.join();
    }

    template <typename T, typename... Args>
    void addTask(T&& f, Args&& ... args) {
        function<void()> task = bind(std::forward<T>(f), std::forward<Args>(args)...);
        {
            unique_lock<mutex> lk(mtx);
            taskQueue.emplace(std::move(task));
        }
        
        cv.notify_one();
    }

};

void task(int& i) {
    cout << "finished task " << i << endl;
}
// void test() {
//     ThreadPool threadpool(5);
//     // 建構子是直接建構 5 個多線程(這裡是消費者模式)來處理任務隊列中的任務
//     // 並利用 cv 來讓他們睡覺 
//     // 等到有任務在任務隊列中 才會被生產者喚起
//     for (int i = 1; i <= 10; i++) {
//         cout << "add task " << i << endl;
//         // 一定要先輸出 add task 再加入任務到隊列
//         // 不然一放任務 線程就會去完成任務而輸出 finished task
//         threadpool.addTask(task, ref(i));
//         // 這裡是生產者模式: 會加入任務到任務隊列中
//         this_thread::sleep_for(std::chrono::milliseconds(1000));
//         // 模擬延遲
//     }
//     // 函數結束後 物件會銷毀 就要讓多線程全部運行完成並且 join (寫在解構子中)
// }


// 生產者 - 消費者模型 : 無多線程版本
// 生產者 - 消費者模型 : 多線程版本


// 設計模式: 單例模式
// 單例模式與多線程: 用 call_once 解決
#endif