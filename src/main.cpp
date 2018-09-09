#include "lrucache.h"

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

typedef LRUCache< int, int, PowerTwoRegisterPolicy> LRUCacheP2; 

int main(int argc, char* argv[]) {

    LRUCacheP2 cache(4);
    int r, cost = 0;
    
    cost += cache.Fetch(2, r);
    cost += cache.Fetch(2, r);
    cost += cache.Fetch(3, r);
    cost += cache.Fetch(3, r);


    exit(0);
}