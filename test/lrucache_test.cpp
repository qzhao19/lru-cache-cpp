#include <gtest/gtest.h>
#include "lru/lrucache.hpp"
#include <thread> 

using namespace std::chrono_literals;

class LRUCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建一个容量为2的缓存用于测试
        cache = std::make_unique<LRUCache<int, std::string>>(2);
    }

    void TearDown() override {
        cache.reset();
    }

    std::unique_ptr<LRUCache<int, std::string>> cache;
    static int deletion_count;
};

int LRUCacheTest::deletion_count = 0;

// 基本功能测试
TEST_F(LRUCacheTest, BasicOperations) {
    // 测试空缓存
    EXPECT_EQ(cache->size(), 0);
    EXPECT_EQ(cache->get(1), nullptr);

    // 测试插入和获取
    cache->put(1, std::make_unique<std::string>("one"));
    EXPECT_EQ(cache->size(), 1);
    EXPECT_EQ(*cache->get(1), "one");

    // 测试更新
    cache->put(1, std::make_unique<std::string>("updated"));
    EXPECT_EQ(*cache->get(1), "updated");
}

// 测试LRU淘汰策略
TEST_F(LRUCacheTest, LRUEvictionPolicy) {
    cache->put(1, std::make_unique<std::string>("one"));
    cache->put(2, std::make_unique<std::string>("two"));
    
    // 访问1使其成为最近使用的
    cache->get(1);
    
    // 插入第三个元素，应该淘汰2
    cache->put(3, std::make_unique<std::string>("three"));
    
    EXPECT_EQ(cache->size(), 2);
    EXPECT_EQ(*cache->get(1), "one");  // 1应该还在
    EXPECT_EQ(*cache->get(3), "three"); // 3应该存在
    EXPECT_EQ(cache->get(2), nullptr);  // 2应该被淘汰
}

// 测试TTL过期功能
TEST_F(LRUCacheTest, TTLExpiration) {
    // 添加一个1ms后过期的条目
    cache->put(1, std::make_unique<std::string>("one"), 1);
    
    // 立即获取应该存在
    EXPECT_EQ(*cache->get(1), "one");
    
    // 等待2ms确保过期
    std::this_thread::sleep_for(2ms);
    
    // 现在应该过期
    EXPECT_EQ(cache->get(1), nullptr);
    EXPECT_EQ(cache->size(), 0);
}

// 测试删除回调
TEST_F(LRUCacheTest, DeletionCallback) {
    int callback_count = 0;
    auto callback = [&](const int& key, const std::string* value) {
        callback_count++;
    };
    
    // 创建带回调的缓存
    auto custom_cache = LRUCache<int, std::string>(2, callback);
    
    custom_cache.put(1, std::make_unique<std::string>("one"));
    custom_cache.put(2, std::make_unique<std::string>("two"));
    
    // 触发淘汰
    custom_cache.put(3, std::make_unique<std::string>("three"));
    
    EXPECT_EQ(callback_count, 1);  // 确保回调被调用一次
    
    // 测试显式删除
    custom_cache.remove(2);
    EXPECT_EQ(callback_count, 2);
    
    // 测试清空
    custom_cache.clear();
    EXPECT_EQ(callback_count, 3);
}

// 测试内存管理
TEST_F(LRUCacheTest, MemoryManagement) {
    struct TestValue {
        ~TestValue() { deletion_count++; }
    };
    
    {
        LRUCache<int, TestValue> test_cache(2);
        test_cache.put(1, std::make_unique<TestValue>());
        test_cache.put(2, std::make_unique<TestValue>());
        
        // 触发淘汰
        test_cache.put(3, std::make_unique<TestValue>());
    } // 析构时应该删除所有元素
    
    EXPECT_EQ(deletion_count, 3);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}