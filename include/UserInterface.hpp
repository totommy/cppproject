#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP

#include <iostream>
#include "./cpphttplib.h"
#include "./json.hpp" // 用來生成 JSON
#include "./File.hpp"
#include "./ThreadPool.hpp"
#include "./LRU.hpp"

using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;

class User {
private:
    size_t capacity;
    LRU<string, File> lru;
    ThreadPool threadpool;
    std::mutex lru_mtx; // << 用來保護 lru 與檔案建立的 race

    // private constructor
    User(size_t capacity, int numThreads) : 
        capacity(capacity), lru(capacity), threadpool(numThreads) {}
public:
    // 合併成一個 getInstance（避免兩個不同 static）
    static User& getInstance(size_t capacity = 5, int numThreads = 3) {
        static User user(capacity, numThreads);
        return user;
    }

    // delete copy
    User(const User&) = delete;
    User& operator=(const User&) = delete;

    // createFile 以 mutex 保護整個 check+create+put 流程
    void createFile(const string& filename) {
        std::lock_guard<std::mutex> g(lru_mtx);

        // check cache
        if (lru.get(filename) != nullptr) {
            cout << "file: " << filename << " is already exist" << endl;
            return;
        }

        string folder = "sss";
        fs::create_directories(folder);
        string path = folder + "/" + filename + ".txt";
        fstream file_stream(path, ios::out | ios::in | ios::trunc);
        if (!file_stream.is_open()) {
            cerr << "Failed to open/create path: " << path << endl;
            return;
        }
        file_stream << "create successfully" << endl;
        file_stream << "Nice" << endl;

        // 建構 File（name 傳值）
        File file(std::move(file_stream), filename);
        lru.put(filename, std::move(file));
        cout << "created file: " << filename << endl;
    }

    void deleteFile(const string& filename) {
        std::lock_guard<std::mutex> g(lru_mtx);
        string folder = "sss";
        string path = folder + "/" + filename + ".txt";

        // 先從 LRU 嘗試取得檔案
        File* fileptr = lru.get(filename);

        if (fileptr == nullptr) {
            // 檔案不在 LRU cache 中
            if (fs::exists(path)) {
                // 重新開啟並刪除
                try {
                    std::ofstream tmp(path, std::ios::in | std::ios::out);
                    tmp.close();
                    if (fs::remove(path))
                        cout << "Remove file (not in cache): " << path << " successfully\n";
                    else
                        cerr << "Fail to remove file (not in cache): " << path << endl;
                } catch (const std::exception& e) {
                    cerr << "Exception while removing " << path << ": " << e.what() << endl;
                }
            } else {
                cout << "File: " << filename << " does NOT exist" << endl;
            }
            return;
        }

        // 檔案在 LRU cache 中
        fstream& file_stream = fileptr->getFstream();
        if (file_stream.is_open()) {
            file_stream.close();
        }

        bool isRemoved = fs::remove(path);
        if (isRemoved)
            cout << "Remove file: " << path << " successfully\n";
        else {
            cerr << "Failed to remove file: " << path << endl;
            return;
        }

        // 從 LRU cache 移除
        auto& mymap = lru.getMymap();
        auto& cacheList = lru.getCache();
        auto it = mymap.find(filename);
        if (it != mymap.end()) {
            cacheList.erase(it->second);
            mymap.erase(it);
        }
    }


    json printTopN() {
        std::lock_guard<std::mutex> g(lru_mtx);
        json arr = json::array();

        for (auto &kv : lru.getCache()) {
            json item;
            item["filename"] = kv.first;
            arr.push_back(item);
        }

        json myjson;
        myjson["cache_size"] = lru.getCache().size();
        myjson["files"] = arr;
        return myjson;
    }

    json printAll() {
        lock_guard<mutex> g(lru_mtx);
        string folder = "sss";  // 目錄名稱
        json arr = json::array();

        try {
            // 檢查目錄是否存在
            if (!fs::exists(folder)) {
                cout << "NOT exist: " << folder << std::endl;
                return arr;
            }

            // 🔹 遍歷所有檔案與子資料夾（遞迴）
            for (const auto& entry : fs::recursive_directory_iterator(folder)) {
                if (fs::is_regular_file(entry.path())) {
                    // std::cout << entry.path() << std::endl;  // 印出完整路徑
                    string filename = fs::path(entry.path()).filename().string();
                    cout << "filename = " << filename << endl;
                    arr.push_back(filename);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
        }
        return arr;
    }
   


    // submitTask: 改為值捕獲 filename，避免 dangling reference
    void submitTask(const string& state, string filename = "") {
        if (state == "CREATE") {
            // 捕獲 filename by value
            threadpool.addTask([this, filename]() {
                this->createFile(filename);
            });
        } else if (state == "DELETE") {
            // 捕獲 filename by value
            threadpool.addTask([this, filename]() {
                this->deleteFile(filename);
            });
        } else if (state == "PRINTTOPN") {
            threadpool.addTask([this]() {
                this->printTopN();
            });
        } else if (state == "PRINTALL") {
            threadpool.addTask([this]() {
                this->printAll();
            });
        }
    }

    
    
    
    json getLRUStatus() {
        std::lock_guard<std::mutex> g(lru_mtx);
        json arr = json::array();
        for (auto &kv : lru.getCache()) {
            json item;
            item["filename"] = kv.first;
            item["create_time"] = kv.second.getCreateTimeString(); // 小心
            arr.push_back(item);
        }
        json resp;
        resp["cache_size"] = lru.getCache().size();
        resp["files"] = arr;
        return resp;
    }
};

#endif