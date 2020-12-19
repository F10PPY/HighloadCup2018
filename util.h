#ifndef UTIL_H
#define UTIL_H
#include "common.h"


#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif
void memUsage();

//inline bool includes (vector<Like> &a,vector<uint32_t> &b)
//{
//      auto && ita=a.begin();
//  for(auto&& itb:b)
//  {
//      if(ita !=a.end()  && itb==ita->id)
//      {
//              ita++;
//       }
//      else return false;
//  }
//  return true;
//}
//inline int *set_intersection(int *first1, int *last1, int *first2, int *last2, int *dest) {
//  first1 = lower_bound(first1, last1, *first2);
//  first2 = lower_bound(first2, last2, *first1);
//  last1  = upper_bound(first1, last1, *(last2 - 1));
//  last2  = upper_bound(first2, last2, *(last1 - 1));
//  while (first1 != last1 && first2 != last2) {
//    if (*first1 < *first2) {
//      ++first1;
//    } else if (*first2 < *first1) {
//      ++first2;
//    } else {
//      *dest = *first1;
//      ++dest;
//      ++first1;
//      ++first2;
//    }
//  }
//  return dest;
//}
/* Converts a hex character to its integer value */
inline char from_hex(char ch) {
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
inline char to_hex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}
inline uint16_t charrGetUint16(const char *src,uint8_t digits=5)noexcept
{
    if(unlikely(!isdigit(*src)))
    {
        return 0;
    }
    uint16_t dest = 0;
    do {
        dest = dest * 10 + (*src++ -'0');
        if(!isdigit(*src))
        {
            return dest;
        }
    }
    while(digits--);
    return dest;
}
inline bool charr2Uint32(char *&src, uint32_t &dest,uint8_t digits=9)noexcept//4x strtol speed, modifies *str//returns number of digits
{

    //  uintmax_t *out=static_cast<uintmax_t*>(integer);
    // uint32_t *out=static_cast<uint32_t*>(integer);
    if(unlikely(!isdigit(*src)))
    {
        return 0;
    }
    // uint32_t n=digits;
    dest = 0;
    do {
        dest = dest * 10 + (*src++ -'0');
        if(!isdigit(*src))
        {
            return true;
            //return n-digits+1;
        }
    }
    while(digits--);
    return true;
    // return  n-digits;
}
inline bool charr2Uint8(char *&src, uint8_t &dest,uint8_t digits=3)noexcept
{
    if(unlikely(!isdigit(*src)))
    {
        return false;
    }
    dest = 0;
    do {
        dest = dest * 10 + (*src++ -'0');
        if(!isdigit(*src))
        {
            return true;
        }
    }
    while(digits--);
    return true;
}

inline void charrCopy(char *&dest,const char *src,uint8_t len)noexcept//same as memcpy, modifies *dest
{
    //    while(len >=8 ) { //speed up for big arrays
    //        *reinterpret_cast<int64_t*>(dest) = *reinterpret_cast<int64_t*>(src);
    //        dest+=8;src+=8;
    //        len-=8;
    //    }
    while (len--)
        *dest++ = *src++;

}
//inline int charrCmp(char* s1,int l1,char *s2,int l2)noexcept//super slow probably bcoz no optimizations
//{
//    while((l1>0)&&(l2>0)){
//        l1--;l2--;
//        if (*s1++ != *s2++)
//            return s1[-1] < s2[-1] ? -1 : 1;
//    }
//    if(l1!=l2){
//        return l1 < l2 ? -1 : 1;
//    }
//    return 0;
//}

inline void addKVInt(StrP &dest,const StrP256Max &key,const uint32_t val)noexcept
{
    charrCopy(dest.str_p,key.str_p,key.len);
    ptrdiff_t pd=to_chars(dest.str_p,dest.str_p+10,val).ptr-dest.str_p;
    dest.str_p+=pd;
    *dest.str_p++=',';
    dest.len+=key.len+pd+1;
}
extern StrP256Max k_premium;
inline void addKVPrem(StrP &dest,const uint32_t finish,const uint32_t start)
{
    charrCopy(dest.str_p,k_premium.str_p,k_premium.len);
    to_chars(dest.str_p-12,dest.str_p-2,start);
    to_chars(dest.str_p-31,dest.str_p-21,finish);
    dest.len+=k_premium.len;
}
inline void addKVStr(StrP &dest,const StrP256Max &key,const StrP256MaxPk &val)noexcept
{
    charrCopy(dest.str_p,key.str_p,key.len);
    *dest.str_p++='\"';
    charrCopy(dest.str_p,val.str_p,val.len);
    *dest.str_p++='\"';
    *dest.str_p++=',';
    dest.len+=key.len+val.len+3;
}
inline void addKVStr(StrP &dest,const StrP256Max &key, char *val_str_p,const uint8_t val_len)noexcept
{
    charrCopy(dest.str_p,key.str_p,key.len);
    *dest.str_p++='\"';
    charrCopy(dest.str_p,val_str_p,val_len);
    *dest.str_p++='\"';
    *dest.str_p++=',';
    dest.len+=key.len+val_len+3;
}
inline void jsonClose(StrP &dest)
{
    *(dest.str_p-1)='}';
    *dest.str_p++=',';
    dest.len++;
}
//bool findNextParam(char *&req);
//void decodeCommaVals(char *&req,vector<StrP256Max>& vec,BufArr&);
bool decodeVals2Vec8(char *&req, vector<uint8_t> &vec,BufArr& arr,ska::flat_hash_map<string,uint8_t> &dic);
void decodeVals2Vec32(char *&req, vector<uint32_t> &vec,BufArr& arr,ska::flat_hash_map<string,uint32_t> &dic);
void decodeValue(char *&pstr, StrP256Max &to, BufArr &arr);
inline bool readValue(char *&req,StrP256Max &to,BufArr &arr)//returns false if if no more params to read
{
    do
    {
        arr.ch()=*req;
    }
    while (*++req!= ' '&&*req!='&');
    to.str_p=arr.curr_ln;
    to.len=arr.getLen();
    arr.endLn();
    return true;
}

inline bool readLikes(char *&req,vector <uint32_t>& vec)
{
    uint32_t num;
    charr2Uint32(req,num);
    vec.push_back(num);
    while (*req!=' '&&*req!='&')
    {

        if(*req=='%')//comma
        {
            charr2Uint32(req+=3,num);
            vec.push_back(num);
        }
        else return false;
        //  req++;
    }
    return true;
}
#ifdef DEBUG
extern std::map<string,uint32_t>filter_param_count;
extern std::map<string,uint32_t>group_param_count;
extern char *last_req;

[[noreturn]]inline void sigHandler(int sig)noexcept
{
    // printf("filter: %zu, group: %zu, recom: %zu, sugg: %zu, new user: %zu, likes: %zu, updates: %zu\ntotal: %lu\n", n_filter,n_group,n_recom,n_sugg,n_new,n_likes,n_update,
    //     n_filter+n_group+n_recom+n_sugg+n_new+n_likes+n_update);
    // fflush(stdout);
    // write(STDOUT_FILENO,last,300);
    printf("\nsig:%s\n", strsignal(sig));


#if defined (DEBUG) &&!defined (MT)
    //printAllCounters();
    st_counter.printAllCollectionsStats();
#endif
    _exit(EXIT_SUCCESS);
}
#endif

inline void initSig()
{
#ifdef DEBUG
    if(SIG_ERR==signal(SIGINT, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGSEGV, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGTERM, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGQUIT, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGABRT, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGPIPE, sigHandler))cout<<strerror(errno)<<endl;
    if(SIG_ERR==signal(SIGHUP, sigHandler))cout<<strerror(errno)<<endl;

#endif
}

#endif // UTIL_H
