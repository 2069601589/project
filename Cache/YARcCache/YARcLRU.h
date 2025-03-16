#pragma once
#include <mutex>
#include <unordered_map>

#include "YARcCacheNode.h"

namespace YCache {
    template<typename Key,typename Value>
    class YARcLRU {
    public:
        using Node= YARcCacheNode<Key,Value>;
        using NodePtr=std::shared_ptr<Node>;
        using NodeMap=std::unordered_map<Key,NodePtr>;

        YARcLRU(size_t mainCapacity,size_t transformThreshold):transformThreshold_(transformThreshold), mainCapacity_(mainCapacity),ghostCapacity_(mainCapacity) {
            initialize();
        }

        bool put(Key key,Value value) {
            if(mainCapacity_==0) return false;
            std::lock_guard<std::mutex> lock(mutex_);
            auto it=mainMap_.find(key);
            if(it!=mainMap_.end()) {
                return updataNode(it->second,value);
            }
            return addNewNode(key,value);
        }

        bool get(Key key,Value value,bool &shouldTransform) {
            if(mainCapacity_==0) return false;
            std::lock_guard<std::mutex> lock(mutex_);
            auto it=mainMap_.find(key);
            if(it!=mainMap_.end()) {
                shouldTransform=updateNodeAccess(it->second);
                moveToFront(it->second);
                return true;
            }
            return false;
        }

        bool checkGhost(Key key) {
            auto it=ghostMap_.find(key);
            if(it!=ghostMap_.end()) {
                removeNode(it->second);
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
                removeLeastNode();
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

        bool updateNodeAccess(NodePtr node) {
            node->incrementAccessCount();
            return node->getAccessCount()>=transformThreshold_;
        }
        bool updataNode(NodePtr node,Value value) {
            node->setValue(value);
            moveToFront(node);
            return true;
        }

        void moveToFront(NodePtr node) {
            removeNode(node);
            addToFront(node);
        }
        void removeNode(NodePtr node) {
            if(!node || !mainHead_ || !mainTail_) return;
            node->prev_->next_=node->next_;
            node->next_->prev_=node->prev_;
        }
        void addToFront(NodePtr node) {
            node->prev_=mainHead_;
            node->next_=mainHead_->next_;
            mainHead_->next_->prev_=node;
            mainHead_->next_=node;
        }

        bool addNewNode(Key key,Value value) {
            //满则删
            if(mainMap_.size()==mainCapacity_) {
                removeLeastNode();
            }
            //创建新节点
            NodePtr newNode=std::make_shared<Node>(key,value);
            //加入双向链表
            addToFront(newNode);
            //加入哈希表
            mainMap_[key]=newNode;
            return true;
        }

        bool removeLeastNode() {
            //找到最少访问的节点
            NodePtr leastNode=mainTail_->prev_;
            if(leastNode==mainHead_) return false;
            //从双向链表中删除
            removeNode(leastNode);
            //在哈希表中删除
            mainMap_.erase(leastNode->getKey());

            //加入ghostMap
            if(ghostMap_.size()==ghostCapacity_) {
                removeOldestGhost();
            }
            addToGhost(leastNode);
            return true;
        }

        void removeOldestGhost() {
            NodePtr oldestGhost=ghostTail_->prev_;
            if(oldestGhost!=ghostTail_) {
                removeNode(oldestGhost);
                ghostMap_.erase(oldestGhost->getKey());
            }
        }

        void addToGhost(NodePtr node) {
            node->accessCount_=1;

            node->prev_=ghostHead_;
            node->next_=ghostHead_->next_;
            ghostHead_->next_->prev_=node;
            ghostHead_->next_=node;

            ghostMap_[node->getKey()]=node;
        }

    private:
        size_t transformThreshold_;
        size_t mainCapacity_;
        size_t ghostCapacity_;
        std::mutex mutex_;

        NodePtr mainHead_;
        NodePtr mainTail_;

        NodePtr ghostHead_;
        NodePtr ghostTail_;

        NodeMap mainMap_;
        NodeMap ghostMap_;

    };

}
