#include <iostream>
#include <random>
#include <vector>
#include <iomanip>
#include <string>
#include <array>

#include "YCachePolicy.h"
#include "YLFuCache.h"
#include "YLRuCache.h"

//辅助函数，输出结果
void printResult(int capacity, const std::vector<int> &get_operations, const std::vector<int> &hits) {
    std::cout << "缓存大小: " << capacity << std::endl;
    std::cout << "LFU - 命中率: " << std::fixed << std::setprecision(2) << (100.0 * hits[0] / get_operations[0]) << "%" <<
            std::endl;
    std::cout << "LRU - 命中率: " << std::fixed << std::setprecision(2) << (100.0 * hits[1] / get_operations[1]) << "%" <<
            std::endl;
}

void testHotData() {
    std::cout << "=== 测试场景1：热点数据访问测试 ===" << std::endl;
    const int CAPACITY = 50; //增加缓存容量
    const int OPERATIONS = 500000; //增加操作次数
    const int HOT_KEYS = 20; //热点数据的数量
    const int COLD_KEYS = 5000; //冷数据数量

    YCache::YLFuCache<int, std::string> lfu(CAPACITY);
    YCache::YHashLRuCache<int, std::string> lru(CAPACITY);
    std::array<YCache::YCachePolicy<int, std::string> *, 2> caches = {&lfu, &lru};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> hits(2, 0);
    std::vector<int> get_operations(2, 0);
     for (int i = 0; i < caches.size(); i++) {
        //先进行一系列put操作
        for (int j = 0; j < OPERATIONS; ++j) {
            int key;
            if (j % 100 < 70) {
                key = gen() % HOT_KEYS;
            } else {
                key = HOT_KEYS + (gen() % COLD_KEYS);
            }
            std::string value = "value" + std::to_string(key);
            caches[i]->put(key, value);
        }

        //然后进行随机get操作
        for (int j = 0; j < OPERATIONS; ++j) {
            int key;
            if (j % 100 < 70) {
                key = gen() % HOT_KEYS;
            } else {
                key = HOT_KEYS + (gen() % COLD_KEYS);
            }
            std::string result;
            get_operations[i]++;
            if (caches[i]->get(key, result)) {
                hits[i]++;
            }
        }
     }
    printResult(CAPACITY, get_operations, hits);
    std::cout << "=== 测试场景1结束 ===" << std::endl;
}

void testLoopData() {
    std::cout << "=== 测试场景2：循环数据访问测试 ===" << std::endl;

    const int CAPACITY = 50; //增加缓存容量
    const int LOOP_SIZE = 500; //循环次数
    const int OPERATIONS = 500000; //操作次数

    YCache::YLFuCache<int, std::string> lfu(CAPACITY);
    YCache::YLRuCache<int, std::string> lru(CAPACITY);
    std::array<YCache::YCachePolicy<int, std::string> *, 2> caches = {&lfu, &lru};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> hits(2, 0);
    std::vector<int> get_operations(2, 0);

    for (int i = 0; i < caches.size(); i++) {
        //先进行一系列put操作
        for (int key = 0; key < LOOP_SIZE; ++key) {
            std::string value = "value" + std::to_string(key);
            caches[i]->put(key, value);
        }
        int current_pos = 0;
        //然后进行随机get操作
        for (int j = 0; j < OPERATIONS; ++j) {
            int key;
            if (j % 100 < 60) {
                //60%顺序扫描
                key = current_pos;
                current_pos = (current_pos + 1) % LOOP_SIZE;
            } else if (j % 100 < 90) {
                //30%随机跳跃
                key = gen() % LOOP_SIZE;
            } else {
                //10%访问范围外数据
                key = LOOP_SIZE + (gen() % LOOP_SIZE);
            }

            std::string result;
            get_operations[i]++;
            if (caches[i]->get(key, result)) {
                hits[i]++;
            }
        }
    }

    printResult(CAPACITY, get_operations, hits);
    std::cout << "=== 测试场景2结束 ===" << std::endl;
}

int main() {
    testHotData();
    testLoopData();
    return 0;
}
