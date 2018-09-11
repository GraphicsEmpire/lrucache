#include <utility>
#include <list>
#include <unordered_map>
#include <atomic>

using namespace std;

/*!
* \brief defines the policy for registering an item in the cache
*/ 
class DefaultCacheRegistrationPolicy
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
template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy = DefaultCacheRegistrationPolicy>
class LRUCache : private RegisterPolicy {
  using RegisterPolicy::Register;
  using RegisterPolicy::Unregister;

public:
LRUCache();
virtual ~LRUCache();

/*!
  * \brief clears the cache content
  */ 
void Clear();

/*!
  * \brief fetch a computed value using the supplied hash
  * \param key the supplied key to fetch a value from the cache
  * \param output_value the output value 
  * \return the cost of accessing the entry, which is 0 for a Hit and 1 otherwise.
  */ 
int Fetch(const KeyType& key, ValueType& output_value);

/*!
  * \brief provides the size of the cache
  * \return the cache size
  */ 
uint32_t GetSize() const;  

/*!
  * \brief computes the hit ratio for the cache      
  */ 
double GetHitRatio() const;

/*!
  * \brief counts the number of fetches 
  */ 
uint64_t GetFetchCount() const;

/*!
  * \brief counts the number of cache hits
  */ 
uint64_t GetHitCount() const;

protected:
  typedef std::pair<KeyType, ValueType> KeyValuePairType;
  std::list< KeyValuePairType > queue_;
  std::unordered_map< KeyType, KeyValuePairType* > map_;    
  std::atomic<uint64_t> fetch_count_;
  std::atomic<uint64_t> hit_count_;
};


template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<Size, KeyType, ValueType, RegisterPolicy>::LRUCache(): fetch_count_(0), hit_count_(0) {

}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
LRUCache<Size, KeyType, ValueType, RegisterPolicy>::~LRUCache() {
  Clear();
}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
void LRUCache<Size, KeyType, ValueType, RegisterPolicy>::Clear() {
  map_.clear();
  queue_.clear();
  fetch_count_ = 0;
  hit_count_ = 0;    
}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
int LRUCache<Size, KeyType, ValueType, RegisterPolicy>::Fetch(const KeyType& key, ValueType& output_value) {
  fetch_count_ ++;

  //check if the key is NOT present in the map. Then register it
  auto it = map_.find(key);
  if(it == map_.end()) {

      //use the supplied register policy to perform value computation
      output_value = Register(key);

      //if we are FULL then remove the LRU item
      if(queue_.size() == Size) {
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

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
unsigned int LRUCache<Size, KeyType, ValueType, RegisterPolicy>::GetSize() const {
  return Size;
}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
double LRUCache<Size, KeyType, ValueType, RegisterPolicy>::GetHitRatio() const {
  return static_cast<double>(hit_count_) / ((fetch_count_ == 0) ? 1.0 : static_cast<double>(fetch_count_));
}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
uint64_t LRUCache<Size, KeyType, ValueType, RegisterPolicy>::GetFetchCount() const {
  return fetch_count_;
}

template < uint32_t Size, typename KeyType, typename ValueType, typename RegisterPolicy>
uint64_t LRUCache<Size, KeyType, ValueType, RegisterPolicy>::GetHitCount() const {
  return hit_count_;
}
