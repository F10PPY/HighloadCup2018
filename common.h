#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/types.h>
#include <vector>
#include <experimental/filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <charconv>
#include <set>
#include <signal.h>
#include <thread>
#include <bitset>
#include <atomic>
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "ntb_util.h"
#include "ngvs_map.h"
#include "flat_hash_map-master/bytell_hash_map.hpp"
#include "flat_hash_map-master/unordered_map.hpp"
#include "flat_hash_map-master/flat_hash_map.hpp"
#include "tsl/htrie_map.h"
#include "index.h"
#include "bm/bm.h"
#include "bm/bmaggregator.h"


namespace fs = std::experimental::filesystem;
using namespace rapidjson;
using namespace std;

extern atomic_uint32_t post_cnt;
#define DEBUG
//#define MT


//#define TEST

#ifdef MT
#define NUM_THREADS 3
#else
#define NUM_THREADS 1
#endif


#ifdef LOCAL
#ifdef TEST
#define DATA_PATH "/mnt/hgfs/Dataset/unz-test"
#else
#define DATA_PATH "/mnt/hgfs/Dataset/unz"
#endif

#else
#define DATA_PATH "unz"
#endif

#ifdef TEST
constexpr const int arr_size=32768;
#else
constexpr const int arr_size=1331200;
#endif
//constexpr const int arr_size=1331200;
extern uint32_t max_id;
extern uint32_t max_id_reverse;
extern ntb::Counter st_counter;
extern bool is_rating;
#ifdef DEBUG
//#define COUNT
//inline void countNoMacro(size_t &n)
//{n++;}
//#ifdef COUNT
//#define count countNoMacro
//#else
//#define count(...)
//#endif
#endif
#define BMSSE42OPT

enum{NONEMPTY_8=UINT8_MAX,NONEMPTY_16=UINT16_MAX,NONEMPTY_32=UINT32_MAX};
enum{EMPTY=0};
enum js_keys{K_ID,K_EMAIL,K_BIRTH,K_INTERESTS,K_SEX,K_PREMIUM,K_STATUS,K_COUNTRY,K_CITY,K_FNAME,K_SNAME,K_PHONE,K_LIKES,K_JOINED};
enum predicate{NONE=0};//,LT,GT,YEAR,TS,EQ=5,NEQ,NUL,ANY,STARTS,DOMAIN=10,CODE,CONTAINS,NOW};
enum status{SINGLE,ENGAGED,COMPLICATED};
enum predicate_sex{MALE,FEMALE};
enum prem{NULL0,NULL1,PREM_NOW};



struct BufArr{//null terminated strings
    // size_t sz;
    // uint8_t str_len;
    char* curr_ln,*origin;
    uint8_t offset=0;
    //  BufArr(size_t _sz, size_t _str_len):sz(_sz),str_len(_str_len)
    BufArr(uint32_t sz)
    {
        curr_ln=origin= new char[sz];
    }
    ~BufArr()
    {
        delete [] origin;
    }
    char* endLn()
    {
        //curr_ln+=sz;
        // str_len=offset;
        curr_ln[offset++]='\0';
        char* ret_str=curr_ln;
        curr_ln+=offset;

        offset=0;
        return ret_str;
    }
    char& ch()
    {
        return curr_ln[offset++];
    }
    void reset()
    {
        curr_ln=origin;
    }
    uint8_t getLen()const
    {
        return offset;
    }
};
extern thread_local BufArr buffer;
struct uint21_t{
    uint32_t x:21;
}__attribute__((packed));
struct StrP
{
    char* str_p=nullptr;
    uint32_t len=0;
    StrP(){}
    StrP(char *p,uint32_t sz):str_p(p),len(sz)
    {}
};
struct StrP256Max//maxlen=256
{
    // bool self=false;
    char* str_p=nullptr;
    uint8_t len=0;
    StrP256Max(){}
    StrP256Max(char *p,uint8_t sz):str_p(p),len(sz)
    {}
    StrP256Max(const char *p,bool nullterm=false)
    {
        len=static_cast<uint8_t>(strlen(p));
        if(nullterm)str_p=new char[len+1];
        else str_p=new char[len];
        memcpy(str_p,p,len);
        if(nullterm)str_p[len]='\0';
        // self=true;
    }
    ~StrP256Max()
    {
        //    if(self)delete[] str_p;
        // if(str_p)delete str_p;//FIX THIS :deletes not only self allocated
    }
};
struct StrP256MaxPk//maxlen=256 packed
{
    char* str_p=nullptr;
    uint8_t len=0;
    StrP256MaxPk(){}
    StrP256MaxPk(char *p,uint8_t sz):str_p(p),len(sz)
    {}
}__attribute__((packed));
struct Like
{
    uint32_t likee_id;//21bit
    int32_t ts;//could be 28
    bool operator < (const Like& rhs) const
    {
        return (likee_id < rhs.likee_id);
    }
}__attribute__((packed));
extern ska::flat_hash_map <string,uint8_t>dic_domain;
extern ska::flat_hash_map <string,uint8_t>dic_country;
extern ska::flat_hash_map <string,uint32_t>dic_city;
extern ska::flat_hash_map <string,uint8_t>dic_interest;
extern ska::flat_hash_map <string,uint8_t>dic_fname;
//extern ska::flat_hash_map <string,uint16_t>dic_sname;
extern tsl::htrie_map<char,uint16_t>dic_sname;
extern uint8_t dic_domain_val;
extern uint8_t dic_country_val;
extern uint32_t dic_city_val;
extern uint8_t dic_interest_val;
extern uint8_t dic_fname_val;
extern uint16_t dic_sname_val;
extern vector<StrP256Max> dic_interest_strings;
extern vector<StrP256Max> dic_country_strings;
extern vector<StrP256Max> dic_city_strings;
extern vector<StrP256Max> dic_fname_strings;
extern vector<StrP256Max> dic_sname_strings;

extern vector<bitset<arr_size>>inters_bs_vec;
extern std::vector<unique_ptr<bm::bvector<>> > bm_inters_vec;
extern std::vector<unique_ptr<bm::bvector<>> > reverse_inters_vec;

struct Liker
{
    uint32_t liker_id;//21bit
    int32_t ts;//could be 28
    bool operator < (const Liker& rhs) const
    {
        return (liker_id < rhs.liker_id);
    }

    Liker& operator=(const Liker& other) // copy assignment
    {
        liker_id=other.liker_id;
        ts=other.ts;
        return *this;
    }
//    Liker& operator=( vector<Liker>& other) // copy assignment
//    {
//        other.emplace_back();
//     //   other.back().liker_id=liker_id;
////        liker_id);
////        liker_id=other.begin()->liker_id;
//        return *this;
//    }
}__attribute__((packed));


struct Account
{
    uint8_t nxt_country_nonempty=0;
    uint8_t nxt_city_nonempty=0;
    uint8_t nxt_phonecode_nonempty=0;
    uint8_t nxt_fname_nonempty=0;
    uint8_t nxt_sname_nonempty=0;
    uint16_t nxt_country=0;//offset
    uint16_t nxt_city=0;//offset
    uint16_t nxt_prem=0;
    uint16_t nxt_prem_now=0;
    uint16_t nxt_fname=0;
    uint16_t nxt_phone_code=0;
    uint16_t nxt_sname=0;
    uint16_t nxt_birth_year=0;


    //array

    uint32_t birth_ts=EMPTY;
    StrP256MaxPk email,phone;
    uint16_t city:11=EMPTY;
    uint16_t phone_code:10=EMPTY;
    uint8_t status:2=0;
    uint8_t email_domain=EMPTY;
    uint8_t birth_year=UINT8_MAX;//remove, check by full ts
    uint8_t country=EMPTY;
    uint8_t fname=EMPTY;
    uint16_t sname=EMPTY;
    bool sex,prem_now=false;
    uint32_t joined=0,prem_start=EMPTY,prem_finish=EMPTY;
    bitset<90> interests_bs;
    vector <Like> likes;
    array<vector <Liker>,2>likers;//male/female
    // unordered_map<uint16_t,uint32_t> likes;//id/ts

};//__attribute__((packed));

extern array<Account,arr_size+1>accounts;
extern uint32_t options_ts;

#endif // COMMON_H
