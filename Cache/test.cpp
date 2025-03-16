#include <iostream>
#include <random>
#include <vector>
#include <iomanip>
#include <string>
#include <array>

#include "YCachePolicy.h"
#include "YLFuCache.h"
#include "YLRuCache.h"
#include "YARcCache/YARcCache.h"

//辅助函数，输出结果
void printResult(int capacity, const std::vector<int> &get_operations, const std::vector<int> &hits) {
    std::cout << "缓存大小: " << capacity << std::endl;
    std::cout << "LFU - 命中率: " << std::fixed << std::setprecision(2) << (100.0 * hits[0] / get_operations[0]) << "%" <<
            std::endl;
    std::cout << "LRU - 命中率: " << std::fixed << std::setprecision(2) << (100.0 * hits[1] / get_operations[1]) << "%" <<
            std::endl;
    std::cout << "ARC - 命中率: " << std::fixed << std::setprecision(2) << (100.0 * hits[2] / get_operations[2]) << "%" <<
            std::endl;
}

void testHotData() {
    std::cout << "=== 测试场景1：热点数据访问测试 ===" << std::endl;
    const int CAPACITY = 50; //增加缓存容量
    const int OPERATIONS = 500000; //增加操作次数
    const int HOT_KEYS = 20; //热点数据的数量
    const int COLD_KEYS = 5000; //冷数据数量

    YCache::YHashLFuCache<int, std::string> lfu(CAPACITY,10);
    YCache::YLRuKCache<int, std::string> lru(CAPACITY);
    YCache::YARcCache<int, std::string> arc(CAPACITY);
    std::array<YCache::YCachePolicy<int, std::string> *, 3> caches = {&lfu, &lru,&arc};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);
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

    YCache::YHashLFuCache<int, std::string> lfu(CAPACITY);
    YCache::YLRuKCache<int, std::string> lru(CAPACITY);
    YCache::YARcCache<int, std::string> arc(CAPACITY);
    std::array<YCache::YCachePolicy<int, std::string> *, 3> caches = {&lfu, &lru,&arc};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);

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

void testShiftData() {
    std::cout << "\n=== 测试场景3：工作负载剧烈变化测试 ===" << std::endl;
    const int CAPACITY = 50; //增加缓存容量
    const int OPERATIONS = 100000; //操作次数

    YCache::YHashLFuCache<int, std::string> lfu(CAPACITY);
    YCache::YLRuKCache<int, std::string> lru(CAPACITY);
    YCache::YARcCache<int, std::string> arc(CAPACITY);
    std::array<YCache::YCachePolicy<int, std::string> *, 3> caches = {&lfu, &lru,&arc};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> hits(3, 0);
    std::vector<int> get_operations(3, 0);
    for(int i=0;i<caches.size();i++) {
        for(int key=0;key<1000;key++) {
            std::string value = "init" + std::to_string(key);
            caches[i]->put(key,value);
        }
        for(int op=0;op<OPERATIONS;op++) {
            int key;
            if(op<OPERATIONS/5) { //热点数据
                key = gen()%5;
            }else if(op<OPERATIONS/5*2) {  //大范围随机
                key = gen()%1000;
            }else if(op<OPERATIONS/5*3) {  //顺序扫描
                key = (op-OPERATIONS/5*2)%100;
            }else if(op<OPERATIONS/5*4) {  //局部随机
                int locality = (op/1000)%10;
                key = locality*20+(gen()%20);
            }else {  //混合
                int r = gen()%100;
                if(r<30) {
                    key = gen()%5;
                }else if(r<60) {
                    key = 5+(gen()%95);
                }else {
                    key = 100+(gen()%900);
                }
            }
            std::string result;
            get_operations[i]++;
            if(caches[i]->get(key,result)) {
                hits[i]++;
            }
            //随机30%put
            if(gen()%100<30) {
                std::string value = "new" + std::to_string(key);
                caches[i]->put(key,value);
            }
        }
    }
    printResult(CAPACITY, get_operations, hits);
    std::cout << "=== 测试场景3结束 ===" << std::endl;
}
int main() {
    testHotData();
    testLoopData();
    testShiftData();
    return 0;
}
