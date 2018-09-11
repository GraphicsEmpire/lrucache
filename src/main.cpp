#include "lrucache.h"
#include <iostream>


class PowerTwoRegisterPolicy
{
protected:
    int Register(const int& key) const
    {                
        return key*key;
    }

    void Unregister(const int& key, const int& value) const
    {                
    }
};

typedef LRUCache< 4, int, int, PowerTwoRegisterPolicy> LRUCacheP2; 

int main(int argc, char* argv[]) {

    LRUCacheP2 cache;
    int r, cost = 0;
    
    for(int i=0; i < 100000; i++) {
        cost += cache.Fetch(2, r);
        cost += cache.Fetch(2, r);
        cost += cache.Fetch(3, r);
        cost += cache.Fetch(3, r);
    }

    std::cout << "Cache Size = " << cache.GetSize() << std::endl;
    std::cout << "Cache Total fetches = " << cache.GetFetchCount() << ", hit count = " << cache.GetHitCount() << std::endl;    
    std::cout << "Cache Hit Ratio = " << cache.GetHitRatio() << std::endl;
    std::cout << "Total access cost = " << cost << std::endl;

    exit(0);
}