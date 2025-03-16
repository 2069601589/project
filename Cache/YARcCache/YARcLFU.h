#pragma once
#include <list>
#include <mutex>
#include <unordered_map>

#include "YARcCacheNode.h"

namespace YCache {
    template<typename Key, typename Value>
    class YARcLFU {
    public:
        using Node =YARcCacheNode<Key,Value>;
        using NodePtr= std::shared_ptr<Node>;
        using NodeMap=std::unordered_map<Key,NodePtr>;
        using FreMap=std::unordered_map<size_t,std::list<NodePtr>>;

        YARcLFU(size_t mainCapacity,size_t transformThreshold): transformThreshold_(transformThreshold), mainCapacity_(mainCapacity),ghostCapacity_(mainCapacity),minFreq_(0) {
            initialize();
        }
        ~YARcLFU()= default;
        bool put(Key key,Value value) {
            std::lock_guard<std::mutex> lock(mutex_);
            if(mainCapacity_<=0) return false;
            auto it=mainMap_.find(key);
            if(it!=mainMap_.end()) {
                return updateNode(it->second,value);
            }
            return addNewNode(key,value);
        }
        bool get(Key key,Value &value) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it=mainMap_.find(key);
            if(it!=mainMap_.end()) {
                updateNodeFreq(it->second);
                value=it->second->getValue();
                return true;
            }
            return false;
        }

        bool checkGhost(Key key) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it=ghostMap_.find(key);
            if(it!=ghostMap_.end()) {
                removeFromGhost(it->second);
                ghostMap_.erase(it);
                return true;
            }
            return false;
        }
        void increaseCapacity() {
            mainCapacity_++;
        }
        bool decreaseCapacity() {
            if(mainCapacity_<=0) return false;
            if(mainMap_.size()==mainCapacity_) {
                removeMainLeast();
            }
            mainCapacity_--;
            return true;
        }
    private:
        void initialize() {
            mainHead_=std::make_shared<Node>();
            mainTail_=std::make_shared<Node>();
            mainHead_->next_=mainTail_;
            mainTail_->prev_=mainHead_;

            ghostHead_=std::make_shared<Node>();
            ghostTail_=std::make_shared<Node>();
            ghostHead_->next_=ghostTail_;
            ghostTail_->prev_=ghostHead_;
        }
        bool updateNode(NodePtr node,Value value) {
            node->setValue(value);
            updateNodeFreq(node);
            return true;
        }
        void updateNodeFreq(NodePtr node) {
            //从原有链表删除
            size_t oldFreq=node->getAccessCount();
            freMap_[oldFreq].remove(node);
            if(freMap_[oldFreq].empty()){
                freMap_.erase(oldFreq);
                if(oldFreq==minFreq_) {
                    minFreq_++;
                }
            }
            node->incrementAccessCount();
            size_t newFreq=node->getAccessCount();
            //如果不存在该频次的链表，则创建
            if(freMap_.find(newFreq)==freMap_.end()) {
                freMap_[newFreq]=std::list<NodePtr>();
            }
            freMap_[newFreq].push_back(node);
        }

        bool addNewNode(Key key,Value value) {
            if(mainMap_.size()>=mainCapacity_) {
                removeMainLeast();
            }
            NodePtr newNode=std::make_shared<Node>(key,value);
            mainMap_[key]=newNode;
            if(freMap_.find(1)==freMap_.end()) {
                freMap_[1]=std::list<NodePtr>();
            }
            freMap_[1].push_back(newNode);
            minFreq_=1;
            return  true;
        }

        void removeMainLeast() {
            if(freMap_.empty()) return;
            //找最小频次的链表
            auto& minFreqList=freMap_[minFreq_];
            if(minFreqList.empty()) return;
            //找最小频次的节点
            NodePtr leastNode=minFreqList.front();
            minFreqList.pop_front();
            if(minFreqList.empty()) {
                freMap_.erase(minFreq_);
                if(!freMap_.empty()) {
                    minFreq_=freMap_.begin()->first;
                }
            }

            //加入ghost链表
            if(ghostMap_.size()>=ghostCapacity_) {
                removeOldestGhost();
            }
            addToGhost(leastNode);

            //从主链表删除
            mainMap_.erase(leastNode->getKey());
        }

        void removeOldestGhost() {
            NodePtr oldestGhost=ghostHead_->next_;
            if(oldestGhost!=ghostTail_) {
                removeFromGhost(oldestGhost);
                ghostMap_.erase(oldestGhost->getKey());
            }
        }
        void removeFromGhost(NodePtr node) {
            node->prev_->next_=node->next_;
            node->next_->prev_=node->prev_;
        }
        void addToGhost(NodePtr node) {
            node->next_=ghostTail_;
            node->prev_=ghostTail_->prev_;
            ghostTail_->prev_->next_=node;
            ghostTail_->prev_=node;
            ghostMap_[node->getKey()]=node;
        }

    private:
        size_t transformThreshold_;
        size_t mainCapacity_;
        size_t ghostCapacity_;
        size_t minFreq_;
        std::mutex mutex_;
        NodeMap mainMap_;
        NodeMap ghostMap_;
        FreMap freMap_;

        NodePtr mainHead_;
        NodePtr mainTail_;

        NodePtr ghostHead_;
        NodePtr ghostTail_;
    };
}
