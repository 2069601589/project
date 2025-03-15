#pragma once
#include <future>
#include <unordered_map>

#include "YCachePolicy.h"

namespace YCache {
    template<typename Key, typename Value>
    //频次双向链表
    class FreList {
    private:
        struct Node {
            int freq;
            Key key;
            Value value;
            std::shared_ptr<Node> pre;
            std::shared_ptr<Node> next;

            Node() : freq(1), pre(nullptr), next(nullptr) {
            }

            Node(Key key, Value value) : freq(1), key(key), value(value), pre(nullptr), next(nullptr) {
            }
        };

        using NodePtr = std::shared_ptr<Node>;
        int freq_;
        NodePtr head_; // 假头结点
        NodePtr tail_; // 假尾结点
    public:
        explicit FreList(int n) : head_(std::make_shared<Node>()), tail_(std::make_shared<Node>(), freq_(n)) {
            head_->next = tail_;
            tail_->pre = head_;
        }

        [[nodiscard]] bool isEmpty() const {
            return head_->next == tail_;
        }

        void addNode(NodePtr node) {
            if (!node || !head_ || !tail_) return;
            node->pre = tail_->pre;
            node->next = tail_;
            tail_->pre->next = node;
            tail_->pre = node;
        }

        void removeNode(NodePtr node) {
            if (!node || !head_ || !tail_) return;
            node->pre->next = node->next;
            node->next->pre = node->pre;
            node->pre == nullptr;
            node->next = nullptr;
        }

        NodePtr getFirstNode() {
            if (isEmpty()) return nullptr;
            return head_->next;
        }
    };

    template<typename Key, typename Value>
    class YLFuCache :public YCachePolicy<Key, Value> {
    public:
        using Node = typename FreList<Key, Value>::Node;
        using NodePtr = std::shared_ptr<Node>;
        using NodeMap = std::unordered_map<Key, NodePtr>;
        YLFuCache(): YLFuCache(10, 10) {};
        YLFuCache(int capacity, int maxAverageNum) : capacity_(capacity), maxAverageNum_(maxAverageNum), curAverageNum_(0), curTotalNum_(0), minFreq_(1) {}
        ~YLFuCache()  override = default;
        void put(Key key,Value value) override {
            if(capacity_==0) return;
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end()) {
                it->second->value = value;
                getInternal(it->second,value);
            } else {
                putInternal(key, value);
            }
        }

        bool get(Key key,Value &value) override {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = nodeMap_.find(key);
            if (it != nodeMap_.end()) {
                value = it->second->value;
                getInternal(it->second,value);
                return true;
            }
            return false;
        }

        Value get(Key key) override {
            Value value;
            get(key,value);
            return value;
        }

    private:
        void putInternal(Key key, Value value); //添加缓存
        void getInternal(NodePtr node, Value &value); //获取缓存

        void kickOut(); //删除最不常访问的结点

        void addToFreqList(NodePtr node); //添加新结点
        void removeFromFreqList(NodePtr node); //从频次链表中删除结点

        void increaseFreqNum(int num); //增加访问频次
        void decreaseFreqNum(int num); //减少访问频次

        void handleOverMaxAverageNum(); //处理超过最大平均访问频次
        void updateMinFreq(); //更新最小访问频次

    private:
        int capacity_; //容量
        int maxAverageNum_; //最大平均访问频次
        int curAverageNum_; //当前平均访问频次
        int curTotalNum_; //当前访问所有缓存次数总数
        int minFreq_; //最小访问频次
        std::mutex mutex_; //互斥锁
        NodeMap nodeMap_; //key到缓存节点的映射
        std::unordered_map<Key,FreList<Key,Value>*> freqToFreqList_; //访问频次到该频次链表的映射
    };

    template<typename Key, typename Value>
    void YLFuCache<Key, Value>::putInternal(Key key, Value value) {
        if (nodeMap_.size() == capacity_) {
            //缓存已满，删除最不常访问的结点，更新当前平均访问频次和总访问频次
            kickOut();
        }
        //创建新结点，将新结点添加进入，更新最小访问频次
        NodePtr node = std::make_shared<Node>(key,value);
        nodeMap_[key] = node;
        addToFreqList(node);
        increaseFreqNum(1);
        minFreq_ = std::min(minFreq_,1);
    }

    template<typename Key, typename Value>
    void YLFuCache<Key, Value>::getInternal(NodePtr node, Value &value) {
        if(!node) return;
        //找到之后需要将其从低访问频次的链表中删除，并且添加到+1的访问频次链表中
        int freq = node->freq;
        removeFromFreqList(node);
        if(freq==minFreq_ && freqToFreqList_[freq]->isEmpty()) minFreq_++;
        ++node->freq;
        addToFreqList(node);
        increaseFreqNum(1);
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::kickOut() {
        NodePtr node = freqToFreqList_[minFreq_]->getFirstNode();
        removeFromFreqList(node);
        nodeMap_.erase(node->key);
        decreaseFreqNum(node->freq);
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::addToFreqList(NodePtr node) {
        int freq=node->freq;
        if(freqToFreqList_.find(freq)==freqToFreqList_.end()) {
            freqToFreqList_[freq] = new FreList<Key,Value>(freq);
        }
        freqToFreqList_[freq]->addNode(node);
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::removeFromFreqList(NodePtr node) {
        int freq=node->freq;
        if(freqToFreqList_.find(freq)==freqToFreqList_.end()) return;
        freqToFreqList_[freq]->removeNode(node);
        //to do 不一定要删除，当元素访问频次较低时，会频繁删除和创建链表
        //可以考虑根据情况加一个阈值，根据实际情况修改
        // //如果频次列表为空，删除该频次列表
        // if(freqToFreqList_[freq]->isEmpty()) {
        //     freqToFreqList_.erase(freq);
        //     if(freq==minFreq_) updateMinFreq();
        // }

    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::increaseFreqNum(int num) {
        curTotalNum_+=num;
        if(nodeMap_.empty()) curAverageNum_=0;
        else curAverageNum_=curTotalNum_/nodeMap_.size();
        if(curAverageNum_>maxAverageNum_) {
            handleOverMaxAverageNum();
        }
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::decreaseFreqNum(int num) {
        curTotalNum_-=num;
        if(nodeMap_.empty()) curAverageNum_=0;
        else curAverageNum_=curTotalNum_/nodeMap_.size();
        if(curAverageNum_>maxAverageNum_) {
            handleOverMaxAverageNum();
        }
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::handleOverMaxAverageNum() {
        if(nodeMap_.empty()) return;
        for(auto it=nodeMap_.begin();it!=nodeMap_.end();++it) {
            if(!it->second) continue;
            NodePtr node = it->second;
            removeFromFreqList(node);
            node->freq-=maxAverageNum_/2;
            if(node->freq<1) node->freq=1;
            addToFreqList(node);
        }
        updateMinFreq();
    }

    template<typename Key, typename Value>
    void YLFuCache<Key,Value>::updateMinFreq() {
        minFreq_=INT8_MAX;
        for(const auto &pair:freqToFreqList_) {
            if(pair.second && !pair.second->isEmpty()) {
                minFreq_=std::min(minFreq_,pair.first);
            }
        }
        if(minFreq_==INT8_MAX) minFreq_=1;
    }
}
