#ifndef NTB_UTIL_H
#define NTB_UTIL_H
#include <string_view>
#include <chrono>
#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std::chrono;

namespace ntb
{
//extern std::chrono::high_resolution_clock::time_point start;
//extern thread_local std::chrono::high_resolution_clock::time_point finish;
typedef std::chrono::duration<float> float_seconds;

class Counter
{
    struct Stats
    {
        uintmax_t min=0,max=0,samples_count=0,average=0,total_ms=0;
        Stats& operator+=(const Stats& rhs)
        {
            min+= rhs.min;
            max+= rhs.max;
            samples_count+= rhs.samples_count;
            average+= rhs.average;
            total_ms+=rhs.total_ms;
            return *this;
        }
    }stats;
    std::string name;
    std::vector<uint32_t> collection;
    std::map<std::string,std::vector<uint32_t>>collections;
    static std::chrono::high_resolution_clock::time_point static_start;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point finish;
public:
    Counter()
    {
        reset();
    }
    Counter(std::string _name):name(_name)
    {
        reset();
    }

    inline static void resetStatic()
    {
        static_start=high_resolution_clock::now();
    }
    inline void reset()
    {
        start=high_resolution_clock::now();
    }
    inline std::string elapsed(std::string_view str="")
    {
        finish=high_resolution_clock::now();
        std::string result("Timestamp \"");
        return result+str.data()+"\": "+(std::to_string(duration_cast<float_seconds> (finish-start).count()))
                .append(" seconds  = " )
                .append(std::to_string(duration_cast<nanoseconds> (finish-start).count())).append(" nanos");

    }
    inline void calcAndPrint()
    {
        calcStats(collection);
        printStats(name,stats);
    }
    inline void printStats(std::string _name,Stats &_stats)
    {
        _name.resize(46,' ');
        std::cout<<_name<<"->ave:\033[33m"<<std::to_string(_stats.average)<<"\033[0m, cnt:\033[31m"<<std::to_string(_stats.samples_count)<<"\033[0m, sum ms:\033[32m"<<std::to_string(_stats.total_ms)<<"\033[0m"<<std::endl;
    }
    inline void addTsMcs()
    {
        finish=high_resolution_clock::now();
        collection.emplace_back(duration_cast<microseconds> (finish-start).count());
    }
    inline void addTsMcs(const std::string &name)
    {
        finish=high_resolution_clock::now();
        collections[name].emplace_back(duration_cast<microseconds> (finish-start).count());
    }
    inline void addStaticTsMcs()
    {
        finish=high_resolution_clock::now();
        collection.emplace_back(duration_cast<microseconds> (finish-static_start).count());
    }
    inline void addStaticTsMcs(const std::string &name)
    {
        finish=high_resolution_clock::now();
        collections[name].emplace_back(duration_cast<microseconds> (finish-static_start).count());
    }
    inline void printAllCollectionsStats()
    {
        Stats totals;
        for(auto&&it:collections)
        {
            totals+=calcStats(it.second);
            printStats(it.first,stats);
        }
        totals.average/=collections.size();
        printStats("Totals",totals);
    }

    inline Stats& calcStats(std::vector<uint32_t> &coll_vec)
    {
        std::vector<uintmax_t> temp,temp2;
        if(coll_vec.empty())
        {
            stats.min=0;stats.max=0;stats.samples_count=0;stats.average=0;stats.total_ms=0;
            return stats;
        }
        temp.reserve(coll_vec.size()/2);
        for(auto it=coll_vec.begin();it!=coll_vec.end();it++)
        {
            if(it+1!=coll_vec.end())
            {
                temp.push_back((*it+*(++it))/2);
            }
            else temp.push_back(*it);
        }
        temp2=std::move(temp);


        while(temp2.size()!=1)
        {
            temp.reserve(temp2.size()/2);
            for(auto it=temp2.begin();it!=temp2.end();it++)
            {
                if(it+1!=temp2.end())
                {
                    temp.push_back((*it+*(++it))/2);
                }
                else temp.push_back(*it);
            }
            temp2=std::move(temp);
        }
        stats.average=temp2[0];
        stats.samples_count=coll_vec.size();
        stats.total_ms=stats.average*stats.samples_count/1000;
        return stats;
    }
    inline void clearCollection()
    {
        collection.clear();
    }
    inline void clearAllCollections()
    {
        collections.clear();
    }
    inline float elapsedSec()
    {
        finish=high_resolution_clock::now();
        return duration_cast<float_seconds> (finish-start).count();
    }
    inline void elapsedPrint(std::string_view str="")
    {
        std::cout<<elapsed(str)<<std::endl;
    }
};

inline void printAll(const char * buff,const size_t bytes) noexcept
{
    for(unsigned int i=0;i<bytes;++i)
    {
        switch (buff[i])
        {
        case '\n':
            puts("\\n");
            break;
        case '\r':
            puts("\\r");
            break;
        case '\t':
            puts("\\t");
            break;
        default:
            //if ((buff[i] < 0x20) || (buff[i] > 0x7f)) {
            //	printf("\\%03o", (unsigned char)buff[i]);
            //	} else {
            //		printf("%c", buff[i]);
            //	}
            printf("%c",buff[i]);
            break;
        }
    }
    fflush(stdout);
}
}//namespace end

#endif // NTB_UTIL_H
