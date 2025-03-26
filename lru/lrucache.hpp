#pragma once
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include "queue.hpp"

template <typename Key, typename Value>
class LRUCache {
    using CallbackType = std::function<void(const Key&, const Value*)>;
    using NodeType = Node<Key, Value>;
    using TimePoint = std::chrono::steady_clock::time_point;

public:
    explicit LRUCache(size_t capacity, CallbackType callback = default_callback) 
        : capacity_(capacity), callback_(std::move(callback)) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Capacity must be greater than zero");
        }
    }

    ~LRUCache() {
        clear();
    }

    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;

    Value* get(const Key& key) {
        auto iter = lookup_table_.find(key);
        if (iter == lookup_table_.end()) {
            return nullptr;
        }

        auto* node = iter->second;
        if (is_expired(node)) {
            evict_node(node);
            return nullptr;
        }

        move_to_head(node);
        return node->value.get(); // 返回原始指针
    }

    void put(const Key& key, std::unique_ptr<Value> value, uint32_t ttl_ms = 0) {
        if (!value) {
            throw std::invalid_argument("Value cannot be null");
        }

        auto iter = lookup_table_.find(key);
        if (iter != lookup_table_.end()) {
            // 更新现有节点
            auto* node = iter->second;
            node->value = std::move(value);
            node->cacheTTL = ttl_ms;
            move_to_head(node);
            return;
        }

        // 检查容量并可能淘汰最久未使用的
        if (lookup_table_.size() >= capacity_) {
            evict_node(cache_list_.tail());
        }

        // 创建新节点
        auto* node = new NodeType(key, std::move(value), ttl_ms);
        cache_list_.insert_head(node);
        lookup_table_[key] = node;
    }

    bool remove(const Key& key) {
        auto iter = lookup_table_.find(key);
        if (iter == lookup_table_.end()) {
            return false;
        }

        auto* node = iter->second;
        evict_node(node);
        return true;
    }

    void clear() {
        cache_list_.foreach([this](const Key& key, Value* val) {
            callback_(key, val);
        });
        cache_list_.clear(); // 假设Queue有clear方法
        lookup_table_.clear();
    }

    size_t size() const { return lookup_table_.size(); }
    size_t capacity() const { return capacity_; }

protected:
    static void default_callback(const Key&, const Value*) {}

private:
    bool is_expired(NodeType* node) const {
        if (node->cacheTTL == 0) return false;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - TimePoint{}).count();
        return node->cacheTTL < elapsed;
    }

    void move_to_head(NodeType* node) {
        cache_list_.remove(node);
        cache_list_.insert_head(node);
    }

    void evict_node(NodeType* node) {
        callback_(node->key, node->value.get());
        cache_list_.remove(node);
        lookup_table_.erase(node->key);
        delete node;
    }

    const size_t capacity_;
    CallbackType callback_;
    
    Queue<Key, Value> cache_list_;
    std::unordered_map<Key, NodeType*> lookup_table_;
};