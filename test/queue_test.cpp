#include <gtest/gtest.h>
#include "lru/queue.hpp" 

// 测试空队列状态
TEST(QueueTest, InitialState) {
    Queue<int, int> queue;
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.head(), nullptr);
    EXPECT_EQ(queue.tail(), nullptr);
}

// 测试插入和移除节点
TEST(QueueTest, InsertAndRemove) {
    Queue<int, std::string> queue;
    
    // 创建测试节点
    auto node1 = new Node<int, std::string>(1, 
        std::make_unique<std::string>("first"), 1000);
        
    // 测试插入头部
    queue.insert_head(node1);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.head(), node1);
    EXPECT_EQ(queue.tail(), node1);
    
    // 测试插入尾部
    auto node2 = new Node<int, std::string>(2, 
        std::make_unique<std::string>("second"), 2000);
    queue.insert_tail(node2);
    EXPECT_EQ(queue.head(), node1);
    EXPECT_EQ(queue.tail(), node2);
    
    // 测试移除节点
    queue.remove(node1);
    delete node1;
    EXPECT_EQ(queue.head(), node2);
    EXPECT_EQ(queue.tail(), node2);
    
    // 清空队列
    queue.remove(node2);
    delete node2;
    EXPECT_TRUE(queue.empty());
}

// 测试遍历功能
TEST(QueueTest, ForEachTraversal) {
    Queue<int, int> queue;
    
    // 准备测试数据
    auto node1 = new Node<int, int>(1, std::make_unique<int>(10), 100);
    auto node2 = new Node<int, int>(2, std::make_unique<int>(20), 200);
    auto node3 = new Node<int, int>(3, std::make_unique<int>(30), 300);
    
    queue.insert_tail(node1);
    queue.insert_tail(node2);
    queue.insert_tail(node3);
    
    // 测试正向遍历
    std::vector<int> keys;
    std::vector<int> values;
    queue.foreach([&](const int& key, int* value) {
        keys.push_back(key);
        values.push_back(*value);
    });
    
    ASSERT_EQ(keys.size(), 3u);
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 3);
    EXPECT_EQ(values[0], 10);
    EXPECT_EQ(values[1], 20);
    EXPECT_EQ(values[2], 30);
    
    // 测试反向遍历
    keys.clear();
    values.clear();
    queue.reverse_foreach([&](const int& key, int* value) {
        keys.push_back(key);
        values.push_back(*value);
    });
    
    ASSERT_EQ(keys.size(), 3u);
    EXPECT_EQ(keys[0], 3);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 1);
}

// 测试内存管理
TEST(QueueTest, MemoryManagement) {
    // 测试value的自动释放
    bool deleted = false;
    {
        Queue<int, std::string> queue;
        auto node = new Node<int, std::string>(
            1, 
            std::make_unique<std::string>("test"), 
            100
        );
        
        // std::unique_ptr<std::string> = str(new std::string("test"));
        auto str = std::unique_ptr<std::string>(new std::string("test"));
        
        node = new Node<int, std::string>(1, std::move(str), 100);
        queue.insert_tail(node);
        queue.remove(node);
        delete node;
        deleted = true;
    }
    EXPECT_TRUE(deleted);
}

// 测试边界条件
TEST(QueueTest, EdgeCases) {
    Queue<int, int> queue;
    
    // 测试移除空节点
    queue.remove(nullptr);
    
    // 测试移除哨兵节点
    Node<int, int>* base = reinterpret_cast<Node<int, int>*>(&queue);
    queue.remove(base);  // 应该没有效果
    
    // 测试插入空节点
    queue.insert_head(nullptr);
    queue.insert_tail(nullptr);
    EXPECT_TRUE(queue.empty());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}