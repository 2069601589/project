#pragma once
#include <future>
#include <pthread.h>
#include <unordered_map>
#include "YCachePolicy.h"

namespace YCache {
    template<typename Key, typename Value>
    class YLRuCache;

    template<typename Key, typename Value>
    class Node {
    private:
        Key key_;
        Value value_;
        std::shared_ptr<Node> prev_;
        std::shared_ptr<Node> next_;

    public:
        Node() : prev_(nullptr), next_(nullptr) {
        }

        Node(Key key, Value value) : key_(key), value_(value), prev_(nullptr), next_(nullptr) {
        }

        Key getKey() const { return key_; }
        Value getValue() const { return value_; }
        void setValue(const Value &value) { this->value_ = value; }
        friend class YLRuCache<Key, Value>;
    };

    template<typename Key, typename Value>
    class YLRuCache : public YCachePolicy<Key, Value> {
    public:
        using NodePtr = std::shared_ptr<Node<Key, Value> >;
        using NodeMap = std::unordered_map<Key, NodePtr>;

        YLRuCache() : YLRuCache(10) {}
        explicit YLRuCache(int capacity): capacity_(capacity) {
            if (capacity <= 0) throw std::invalid_argument("Capacity must be positive");
            initialization();
        }

        ~YLRuCache() override = default;

        void put(Key key, Value value) override {
            if (capacity_ <= 0) return;
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end()) {
                it->second->value_ = value;
                moveToTail(it->second);
            } else {
                addNewNode(key, value);
            }
        }

        bool get(Key key, Value &value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end()) {
                moveToTail(it->second);
                value = it->second->value_;
                return true;
            }
            return false;
        }

        Value get(Key key) override {
            Value value;
            get(key, value);
            return value;
        }
        void remove(Key key) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end()) {
                removeNode(it->second);
                nodeMap_.erase(it);
            }
        }

    private:
        void initialization();  // 初始化
        void addToTail(NodePtr node);  // 添加到尾部
        void addNewNode(Key key, Value value); // 添加新节点
        void moveToTail(NodePtr node);  // 移动到尾部
        void removeHead();  // 移除头部
        void removeNode(NodePtr node); // 移除节点
    private:
        size_t capacity_; // 缓存容量
        NodeMap nodeMap_;
        std::mutex mutex_;
        NodePtr dummyHead_; // 虚拟头结点
        NodePtr dummyTail_; // 虚拟尾结点
    };

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::initialization() {
        dummyHead_ = std::make_shared<Node<Key, Value> >(Key(), Value());
        dummyTail_ = std::make_shared<Node<Key, Value> >(Key(), Value());
        dummyHead_->next_ = dummyTail_;
        dummyTail_->prev_ = dummyHead_;
    }

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::addToTail(NodePtr node) {
        if (!node || !dummyHead_ || !dummyTail_) return;
        node->prev_ = dummyTail_->prev_;
        node->next_ = dummyTail_;
        dummyTail_->prev_->next_ = node;
        dummyTail_->prev_ = node;
    }

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::addNewNode(Key key, Value value) {
        if (nodeMap_.size() >= capacity_) {
            removeHead();
        }
        NodePtr newNode = std::make_shared<Node<Key, Value> >(key, value);
        addToTail(newNode);
        nodeMap_[key] = newNode;
    }

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::removeNode(NodePtr node) {
        if (!node) return;
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
    }

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::removeHead() {
        if (dummyHead_->next_ == dummyTail_) return;
        NodePtr node = dummyHead_->next_;
        removeNode(node);
        nodeMap_.erase(node->getKey());
    }

    template<typename Key, typename Value>
    void YLRuCache<Key, Value>::moveToTail(NodePtr node) {
        if (!node) return;
        removeNode(node);
        addToTail(node);
    }

    //优化：K-LRU
    template <typename Key, typename Value>
    class YLRuKCache : public YLRuCache<Key, Value> {
    public:
        explicit YLRuKCache(int capacity) : YLRuKCache<Key, Value>(capacity,5*capacity,2) {}
        YLRuKCache(int capacity,int historyCapacity,int k)
        :YLRuCache<Key,Value>(capacity),historyList_(std::make_unique<YLRuCache<Key,size_t> >(historyCapacity)),k_(k) {}
        void put(Key key,Value value) {
            if(YLRuCache<Key,Value>::get(key,value)) {
                YLRuCache<Key,Value>::put(key,value);
                return;
            }
            size_t historyCount = historyList_->get(key);
            historyList_->put(key,++historyCount);
            if(historyCount>=k_) {
                historyList_->remove(key);
                YLRuCache<Key,Value>::put(key,value);
            }
        }
        bool get(Key key,Value &value) {
            int historyCount = historyList_->get(key);
            historyList_->put(key,++historyCount);
            return YLRuCache<Key,Value>::get(key,value);
        }
        Value get(Key key) {
            Value value;
            get(key,value);
            return value;
        }
    private:
        size_t k_; // 进入缓存队列的评判标准
        std::unique_ptr<YLRuCache<Key, size_t> > historyList_;
    };


    //优化：哈希分片
    template<typename Key,typename Value>
    class YHashLRuCache :public YCachePolicy<Key,Value>{
    public:
        explicit YHashLRuCache(int capacity) :YHashLRuCache(capacity,5){}
        YHashLRuCache(int capacity,int sliceNum):capacity_(capacity),sliceNum_(sliceNum) {
            for(int i=0;i<sliceNum_;++i) {
                hashlrucaches_.emplace_back(std::make_unique<YLRuCache<Key,Value> >(capacity/sliceNum));
            }
        }
        void put(Key key,Value value) override{
            size_t hashValue = Hash(key)%sliceNum_;
            hashlrucaches_[hashValue]->put(key,value);
        }
        bool get(Key key,Value &value) override{
            size_t hashValue = Hash(key)%sliceNum_;
            return hashlrucaches_[hashValue]->get(key,value);
        }
        Value get(Key key) override{
            Value value;
            get(key,value);
            return value;
        }
    private:
        size_t Hash(Key key) {
            std::hash<Key> hashFunc;
            return hashFunc(key);
        }
        size_t capacity_;
        size_t sliceNum_;
        std::vector<std::unique_ptr<YLRuCache<Key,Value> > > hashlrucaches_;
    };
}
