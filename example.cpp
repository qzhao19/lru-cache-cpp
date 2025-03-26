#include <iostream>
#include <memory>
#include <thread>
#include "lru/lrucache.hpp"

int main() {
    // 1. 创建一个容量为3的LRU缓存
    LRUCache<int, std::string> cache(3);

    // 2. 添加数据到缓存
    cache.put(1, std::make_unique<std::string>("Apple"));
    cache.put(2, std::make_unique<std::string>("Banana"));
    cache.put(3, std::make_unique<std::string>("Cherry"));

    std::cout << "Initial cache contents (size=" << cache.size() << "):" << std::endl;
    
    // 3. 测试获取数据
    auto printValue = [](int key, const std::string* value) {
        if (value) {
            std::cout << "Key " << key << ": " << *value << std::endl;
        } else {
            std::cout << "Key " << key << ": Not found" << std::endl;
        }
    };

    printValue(1, cache.get(1));  // 应该存在
    printValue(2, cache.get(2));  // 应该存在
    printValue(4, cache.get(4));  // 不存在

    // 4. 测试LRU淘汰策略
    std::cout << "\nAdding one more item (should evict least recently used):" << std::endl;
    cache.put(4, std::make_unique<std::string>("Date"));

    printValue(1, cache.get(1));  // 应该被淘汰
    printValue(2, cache.get(2));  // 应该存在
    printValue(3, cache.get(3));  // 应该存在
    printValue(4, cache.get(4));  // 新添加的

    // 5. 测试TTL过期功能
    std::cout << "\nTesting TTL expiration:" << std::endl;
    cache.put(5, std::make_unique<std::string>("Elderberry"), 1000); // 1秒后过期
    
    std::cout << "Immediately after insertion:" << std::endl;
    printValue(5, cache.get(5));  // 应该存在

    std::cout << "Waiting for expiration..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "After expiration:" << std::endl;
    printValue(5, cache.get(5));  // 应该已过期

    // 6. 测试删除操作
    std::cout << "\nTesting remove operation:" << std::endl;
    cache.remove(3);
    printValue(3, cache.get(3));  // 应该不存在

    return 0;
}