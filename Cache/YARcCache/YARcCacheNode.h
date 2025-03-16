#pragma once
#include <memory>

namespace YCache {
    template <typename Key, typename Value>
    class YARcCacheNode {
    public:
        YARcCacheNode() : accessCount_(1), prev_(nullptr), next_(nullptr){};
        YARcCacheNode(Key key,Value value):key_(key),value_(value),accessCount_(1),prev_(nullptr),next_(nullptr){}

        //Getters
        Key getKey() const {return key_;}
        Value getValue() const {return value_;}
        size_t getAccessCount(){return accessCount_;}

        //Setters
        void setValue(const Value& value){value_=value;}
        void incrementAccessCount(){++accessCount_;}

    private:
        Key key_;
        Value value_;
        size_t accessCount_;
        std::shared_ptr<YARcCacheNode> prev_;
        std::shared_ptr<YARcCacheNode> next_;

        template<typename K,typename V> friend class YARcLRU;
        template<typename K,typename V> friend class YARcLFU;
    };
}
