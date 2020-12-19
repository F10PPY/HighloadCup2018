#ifndef INDEX_H
#define INDEX_H
#include "flat_hash_map-master/bytell_hash_map.hpp"
#include "array"
using namespace std;
enum next_elem{nxt_fullscan,nxt_country,nxt_country_nonempty,nxt_city,nxt_city_any,nxt_city_nonempty,nxt_phonecode,nxt_phonecode_nonempty,
               nxt_prem,nxt_prem_now,nxt_fname,nxt_fname_nonempty,nxt_fname_any,nxt_sname,nxt_sname_nonempty,nxt_sname_starts,
               nxt_birth_year,nxt_interests,nxt_combined,
               nxt_arr_size};
extern ska::flat_hash_map<uint8_t,uint32_t>first_country_map;//country-offset
extern ska::flat_hash_map<uint8_t,uint32_t>first_fname_map;
extern ska::flat_hash_map<uint16_t,uint32_t>first_sname_map;
extern ska::flat_hash_map<uint16_t,uint32_t>first_city_map;//city-offset
extern ska::flat_hash_map<uint16_t,uint32_t>first_phonecode_map;//city-offset
extern array<uint32_t,3>first_prem_arr;
extern array<uint32_t,55>first_birth_arr;
extern ska::bytell_hash_set<string_view>email_set;
void makeIndex();
#endif // INDEX_H
