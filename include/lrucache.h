#include <utility>
#include <list>
#include <unordered_map>
#include <atomic>

using namespace std;

class DefaultRegisterPolicy
{
protected:
    template<typename KeyType, typename ValueType>
    ValueType Register(const KeyType& key) const
    {                
    }

    template<typename KeyType, typename ValueType>
    void Unregister(const KeyType& key, const ValueType& value) const
    {                
    }
};

/*!
 * \brief a fixed-size cache storage with least recently used retainment policy.
 * To fetch an item, the user supplies the key. If the value is stored in the cache, it is
 * returned immediately from cache, otherwise the system registers the value in the cache 
 * in the next available memory slot. In case no memory slots are available, the least recently 
 * used item is removed from the list to accomodate the new value.
 */ 
template < typename KeyType, typename ValueType, typename RegisterPolicy = DefaultRegisterPolicy>
class LRUCache : private RegisterPolicy {
    using RegisterPolicy::Register;
    using RegisterPolicy::Unregister;

public:
  LRUCache(unsigned int size);
  virtual ~LRUCache();

  void Clear();

  int Fetch(const KeyType& key, ValueType& output_value);

  unsigned int GetSize() const;  

  double GetHitRatio() const;
  uint64_t GetFetchCount() const;
  uint64_t GetHitCount() const;

protected:
    typedef std::pair<KeyType, ValueType> KeyValuePairType;
    std::list< KeyValuePairType > queue_;
    std::unordered_map< KeyType, KeyValuePairType* > map_;    
    std::atomic<uint64_t> fetch_count_;
    std::atomic<uint64_t> hit_count_;

    unsigned int size_;
};


template < typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<KeyType, ValueType, RegisterPolicy>::LRUCache(unsigned int size): size_(size), fetch_count_(0), hit_count_(0) {

}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<KeyType, ValueType, RegisterPolicy>::~LRUCache() {
    Clear();
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
void LRUCache<KeyType, ValueType, RegisterPolicy>::Clear() {
    map_.clear();
    queue_.clear();
    fetch_count_ = 0;
    hit_count_ = 0;    
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
int LRUCache<KeyType, ValueType, RegisterPolicy>::Fetch(const KeyType& key, ValueType& output_value) {
    fetch_count_ ++;

    //check if the key is NOT present in the map. Then register it
    auto it = map_.find(key);
    if(it == map_.end()) {

        //use the supplied register policy to perform value computation
        output_value = Register(key);

        //if we are FULL then remove the LRU item
        if(queue_.size() == size_) {
            //Unregister the LRU item
            Unregister(queue_.back().first, queue_.back().second);

            //remove LRU item from map by key
            map_.erase(queue_.back().first);

            //remove LRU item from queue
            queue_.pop_back();
        }

        queue_.push_front(std::make_pair(key, output_value));
        map_.insert(std::make_pair(key, &queue_.front()));
        return 1;
    }
    else {
        hit_count_ ++;
        output_value = (*it).second->second;
        return 0;
    }
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
unsigned int LRUCache<KeyType, ValueType, RegisterPolicy>::GetSize() const {
    return size_;
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
double LRUCache<KeyType, ValueType, RegisterPolicy>::GetHitRatio() const {
    return static_cast<double>(hit_count_) / ((fetch_count_ == 0) ? 1.0 : static_cast<double>(fetch_count_));
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
uint64_t LRUCache<KeyType, ValueType, RegisterPolicy>::GetFetchCount() const {
    return fetch_count_;
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
uint64_t LRUCache<KeyType, ValueType, RegisterPolicy>::GetHitCount() const {
    return hit_count_;
}
