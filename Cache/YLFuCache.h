#pragma once
#include <future>

namespace YCache {
    template <typename Key, typename Value>
    //频次双向链表
    class FreList {
    private:
        struct Node {
            int freq;
            Key key;
            Value value;
            std::shared_ptr<Node> pre;
            std::shared_ptr<Node> next;
            Node() : freq(1), pre(nullptr), next(nullptr) {}
            Node(Key key, Value value) : freq(1), key(key), value(value), pre(nullptr), next(nullptr) {}
        };
        using NodePtr = std::shared_ptr<Node>;
        int freq_;
        NodePtr head_; // 假头结点
        NodePtr tail_; // 假尾结点
    public:
        explicit FreList(int n) : head_(std::make_shared<Node>()), tail_(std::make_shared<Node>(), freq_(n)) {

            head_->next=tail_;
            tail_->pre=head_;
        }
        [[nodiscard]] bool isEmpty() const {
            return head_->next==tail_;
        }
        void addNode(NodePtr node) {
            if(!node||!head_||!tail_) return;
            node->pre=tail_->pre;
            node->next=tail_;
            tail_->pre->next=node;
            tail_->pre=node;
        }
        void removeNode(NodePtr node) {
           if(!node||!head_||!tail_) return;
            node->pre->next=node->next;
            node->next->pre=node->pre;
            node->pre==nullptr;
            node->next=nullptr;
        }
        NodePtr getFirstNode() {
            if(isEmpty()) return nullptr;
            return head_->next;
        }

    };


    class YLFuCache {

    };
}




