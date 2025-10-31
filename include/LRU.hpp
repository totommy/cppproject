#ifndef LRU_HPP
#define LRU_HPP

#include  <unordered_map>
#include <list>

using namespace std;

// 模板化 LRUt
template <typename K, typename V>
class LRU {
private:
    size_t capacity;
    list<pair<K, V>> cache;
    unordered_map<K, typename list<pair<K, V>>::iterator> mymap;
public:
    LRU(size_t capacity) : capacity(capacity) {}
    V* get (const K& key) {
        auto it = mymap.find(key);
        if (it == mymap.end()) return nullptr;
        auto list_it = it->second;
        // 把 node splice 到 head
        cache.splice(cache.begin(), cache, list_it);
        // it->second 已被移到 begin，更新 map iterator
        mymap[key] = cache.begin();
        return &(cache.begin()->second);
    }

    void put(const K& key, V&& value) {
        auto it = mymap.find(key);
        if (it != mymap.end()) {
            auto list_it = it->second;
            list_it->second = std::move(value);
            cache.splice(cache.begin(), cache, list_it);
            mymap[key] = cache.begin();
        } else {
            cache.emplace_front(key, std::move(value));
            mymap[key] = cache.begin();
            if (cache.size() > capacity) {
                // 這裡要小心 大於 capacity 的 File 的 fstream 要 close 不然沒辦法 delete
                auto& last = cache.back();
                fstream& file_stream = last.second.getFstream();
                file_stream.close();

                mymap.erase(last.first);
                cache.pop_back();
            }
        }
    }

    void printCache() {
        for (auto& ele : cache) {
            cout << "key = (" << ele.first << ")," << endl;
            cout << "value = (" << ele.second << ")" << endl; 
        }
        cout << "-----" << endl;
    }

    list<pair<K, V>>& getCache() { return cache; }
    unordered_map<K, typename list<pair<K, V>>::iterator>& getMymap() { return mymap; }
};


#endif