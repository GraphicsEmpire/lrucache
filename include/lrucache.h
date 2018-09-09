#include <utility>
#include <list>
#include <unordered_map>

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
template < typename KeyType, typename ValueType, typename RegisterPolicy>
class LRUCache : private RegisterPolicy {
    using RegisterPolicy::Register;
    using RegisterPolicy::Unregister;

public:
  LRUCache(unsigned int size);
  virtual ~LRUCache();

  int Fetch(const KeyType& key, ValueType& output_value);

protected:
    unsigned int size_;

    typedef std::pair<KeyType, ValueType> KeyValuePairType;
    std::list< KeyValuePairType > queue_;
    std::unordered_map< KeyType, KeyValuePairType* > map_;    
};


template < typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<KeyType, ValueType, RegisterPolicy>::LRUCache(unsigned int size): size_(size) {

}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<KeyType, ValueType, RegisterPolicy>::~LRUCache() {
    map_.clear();
    queue_.clear();
}

template < typename KeyType, typename ValueType, typename RegisterPolicy>
int LRUCache<KeyType, ValueType, RegisterPolicy>::Fetch(const KeyType& key, ValueType& output_value) {

    //check if the key is NOT present in the map. Then register it
    auto it = map_.find(key);
    if(it == map_.end()) {
        output_value = Register(key);
        if(queue_.size() == size_) {
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
        output_value = (*it).second->second;
        return 0;
    }
}
