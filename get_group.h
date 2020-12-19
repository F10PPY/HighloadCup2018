#ifndef GET_GROUP_H
#define GET_GROUP_H
#include "util.h"
//vector<uint21_t>Country_birth;
//vector<ska::bytell_hash_map<uint8_t,uint32_t>>Country_birth;
//grp_keys sex, status, interests, country, city.
//grp_fields sex, status, country, city, birth, interests, likes, joined

//struct grp_k_country
//{
//    array<uint16_t,55>birth;
//    array<uint32_t,2>sex;
//    array <uint32_t,3>status;
//    ska::bytell_hash_map<uint32_t,uint16_t> likes;//like-count
//    ska::bytell_hash_map<uint32_t,uint16_t>joined;//joined-count
//    vector<uint32_t>city;
//};

struct KCountry
{
    uint16_t num=0;
};

struct FStatus
{
    uint16_t num=0;
};
struct FSex
{
   ngvs_map<uint16_t,uint16_t>grp_k_city;
   ngvs_map<uint8_t,uint32_t>grp_k_country;
};
struct FInterests
{
    uint16_t num=0;
};
struct FCity
{
  uint16_t num=0;
};
struct FJoined
{
   // array<FSex,2>f_sex;
   // vector<FInterests>f_interests;
   // vector<FCity>f_city;
  //  array<FStatus,3>f_status;
    ngvs_map<uint8_t,uint16_t>grp_k_country;
    ngvs_map<bool,uint32_t>grp_k_sex;

   };
struct grp
{
    array<FJoined,8>f_joined;
    array <FSex,2>f_sex;
};
extern grp groups;


#endif // GET_GROUP_H
