#pragma once
#include <memory>

#include"../YCachePolicy.h"
#include "YARcLFU.h"
#include "YARcLRU.h"

namespace YCache {
    template<typename Key, typename Value>
    class YARcCache : public YCachePolicy<Key, Value> {
    public:
        YARcCache(size_t capacity = 10, size_t transformThreshold = 2):
        capacity_(capacity),
        transformThreshold_(transformThreshold),
        LFU_(std::make_unique<YARcLFU<Key, Value> >(capacity, transformThreshold)),
        LRU_(std::make_unique<YARcLRU<Key, Value> >(capacity, transformThreshold)) {}

        ~YARcCache() override = default;

        void put(Key key, Value value) override {
            bool isGhost=checkGhost(key);
            if(!isGhost) {
                if(LRU_->put(key,value)) {
                    LFU_->put(key,value);
                }
            }else {
                LRU_->put(key,value);
            }
        }

        bool get(Key key, Value &value) override {
            checkGhost(key);
            bool shouldTransform=false;
            if(LRU_->get(key,value,shouldTransform)) {
                if(shouldTransform) LFU_->put(key,value);
                return true;
            }
            return LFU_->get(key,value);
        }

        Value get(Key key) override {
            Value value;
            get(key,value);
            return value;
        }

    private:
        bool checkGhost(Key key) {
            bool inGhost=false;
            if(LRU_->checkGhost(key)) {
                inGhost=true;
                if(LFU_->decreaseCapacity()) {
                    LRU_->increaseCapacity();
                }
            }else if(LFU_->checkGhost(key)) {
                inGhost=true;
                if(LRU_->decreaseCapacity()) {
                    LFU_->increaseCapacity();
                }
            }
            return inGhost;
        }

    private:
        size_t capacity_;
        size_t transformThreshold_;

        std::unique_ptr<YARcLFU<Key, Value> > LFU_{};
        std::unique_ptr<YARcLRU<Key, Value> > LRU_{};
    };
}
