#ifndef NGVS_MAP_H
#define NGVS_MAP_H
#include <vector>
#include <algorithm>
#include  <cstdlib>
#include <iostream>

template <typename K,typename V>
class ngvs_map
{
    std::vector<K>keys;
    u_int32_t first_nonzero_val_pos=0;
public:
    std::vector<std::pair<K,V>>vals;
    using const_it = typename std::vector<std::pair<K,V>>::const_iterator;
    using const_reverse_it = typename std::vector<std::pair<K,V>>::const_reverse_iterator;
    ngvs_map()
    {}
    ngvs_map(size_t size)
    {
        keys.resize(size);
        vals.resize(size);
        for(size_t i=0;i<keys.size();i++)
        {
            keys[i]=i;
            vals[i].first=i;
        }
    }
    const_it begin() const
    {
        return vals.begin()+first_nonzero_val_pos;
    }
    const_it end() const
    {
        return vals.end();
    }
    const_reverse_it rbegin() const
    {
        return vals.rbegin();
    }
    const_reverse_it rend() const
    {
        return vals.rend()-first_nonzero_val_pos;
    }
    std::pair<K,V> & back()
    {
        return vals.back();
    }
    std::pair<K,V> & front()
    {
        return vals.front()+first_nonzero_val_pos;
    }
    //    void emplace(const K key,const V val)
    //    {
    //        if(keys.size()<key+1)
    //        {
    //            K old_sz=keys.size();
    //            keys.resize(key+1);
    //            vals.resize(key+1);

    //            for(;old_sz<keys.size();old_sz++)
    //            {
    //                keys[old_sz]=old_sz;
    //                vals[old_sz].first=old_sz;
    //            }
    //        }
    ////        else
    ////        {
    ////            keys[key]=key;
    ////            vals[key].first=key;
    ////        }
    //        vals[keys[key]].second=val;
    //    }
    void sort()
    {
        std::sort(vals.begin(), vals.end(), [](auto &left, auto &right) {
            return left.second < right.second;
        });
        for(size_t i=0;i<keys.size();i++)
        {
            keys[vals[i].first]=i;
            if(0==vals[i].second)
            {
                first_nonzero_val_pos++;
            }
        }
    }

    //  std::pair<K,V>& operator[](K i) { return &vals[keys[i]]; }

    std::pair<K,V> &operator[](size_t key)
    {
        if(keys.size()<key+1)
        {
            size_t old_sz=keys.size();
            keys.resize(key+1);
            vals.resize(key+1);

            for(;old_sz<keys.size();old_sz++)
            {
                keys[old_sz]=old_sz;
                vals[old_sz].first=old_sz;
            }
        }
        return vals[keys[key]];
    }
    //    std::pair<K,V>& getKV(size_t i)
    //    {
    //        return vals[i];
    //    }
    void inc(K key)//works correct only on the sorted container,doesn't prevent K_MAX++
    {
        if(keys[key]==keys.size()-1||vals[keys[key]].second<vals[keys[key]+1].second)
        {
            vals[keys[key]].second++;
            return;
        }
        else// increased element is more than the right neighbour, so just increase next element and swap keys
        {
            vals[keys[key]+1].second++;

            K temp=vals[keys[key]].first;
            vals[keys[key]].first=vals[keys[key]+1].first;
            vals[keys[key]+1].first=temp;

            keys[vals[keys[key]].first]--;
            keys[key]++;
        }

        // while(vals[keys[++key]]); CHECK NEXT ELEMENTS TOO
    }
    void dec(K key)//works correct only on the sorted container,doesn't prevent 0--
    {
        if(keys[key]==0||vals[keys[key]].second>vals[keys[key]-1].second)
        {
            vals[keys[key]].second--;
        }
        else// deacreased element is less than the left neighbour, so just decrease prev element and swap keys
        {
            vals[keys[key]-1].second--;

            K temp=vals[keys[key]].first;
            vals[keys[key]].first=vals[keys[key]-1].first;
            vals[keys[key]-1].first=temp;

            keys[vals[keys[key]].first]++;
            keys[key]--;
        }
    }
};




#endif // NGVS_MAP_H
