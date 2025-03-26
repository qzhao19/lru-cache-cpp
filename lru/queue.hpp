#pragma once
#include <memory>
#include <functional>
#include <cstdint>

template <typename Key, typename Value>
struct Node {
    Key key;
    std::unique_ptr<Value> value;  // 使用智能指针管理内存
    Node* next;
    Node* prev;
    uint32_t cacheTTL;
    
    Node() : prev(nullptr), next(nullptr), cacheTTL(0), value(nullptr) {};
    Node(const Key& key, std::unique_ptr<Value> value, uint32_t cacheTTL)
        : key(key), 
          value(std::move(value)), 
          next(nullptr), 
          prev(nullptr), 
          cacheTTL(cacheTTL) {}
};

/**
 * 双向循环链表实现队列:
 * 
 *        base_
 *         ↓
 * [head] <--> [node] <--> ... <--> [tail]
 *   ↑                             ↑
 *  head()                        tail()
 */
template <typename Key, typename Value>
class Queue {
public:
    using NodeType = Node<Key, Value>;
    
    Queue() {
        base_.prev = &base_; 
        base_.next = &base_; 
    }

    ~Queue() {
        clear();
    }

    // 清空队列
    void clear() {
        while (!empty()) {
            NodeType* node = head();
            remove(node);
            delete node;
        }
    }

    bool empty() const {  // 添加 const 修饰
        return base_.next == &base_;
    }

    // 安全移除节点
    void remove(NodeType* node) {
        if (node == &base_) return;  // 防止删除哨兵节点
        
        node->prev->next = node->next;
        node->next->prev = node->prev;
        
        // 重置指针防止悬垂
        node->prev = nullptr;
        node->next = nullptr;
    }

    NodeType* tail() const {
        return base_.prev == &base_ ? nullptr : base_.prev;
    }

    NodeType* head() const {
        return base_.next == &base_ ? nullptr : base_.next;
    }

    // 在尾部插入节点
    void insert_tail(NodeType* node) {
        if (!node) return;
        
        node->prev = base_.prev;
        node->next = &base_;
        base_.prev->next = node;
        base_.prev = node;  // 修复：更新base_.prev而不是base_.next
    }

    // 在头部插入节点
    void insert_head(NodeType* node) {
        if (!node) return;
        
        node->prev = &base_;
        node->next = base_.next;
        base_.next->prev = node;
        base_.next = node;
    }

    // 遍历队列（从头到尾）
    void foreach(const std::function<void(const Key&, Value*)>& func) {
        for (auto* node = head(); node != nullptr; node = (node->next == &base_) ? nullptr : node->next) {
            if (node->value) {
                func(node->key, node->value.get());
            }
        }
    }

    // 反向遍历（从尾到头）
    void reverse_foreach(const std::function<void(const Key&, Value*)>& func) {
        for (auto* node = tail(); node != nullptr; node = (node->prev == &base_) ? nullptr : node->prev) {
            if (node->value) {
                func(node->key, node->value.get());
            }
        }
    }

private:
    NodeType base_;  // 哨兵节点
};