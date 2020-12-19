#include "get_group.h"
#include "ngvs_map.h"
ska::flat_hash_map<uint8_t,uint32_t>first_country_map;
ska::flat_hash_map<uint8_t,uint32_t>first_fname_map;
ska::flat_hash_map<uint16_t,uint32_t>first_sname_map;
ska::flat_hash_map<uint16_t,uint32_t>first_city_map;
ska::flat_hash_map<uint16_t,uint32_t>first_phonecode_map;

array<uint32_t,55>first_birth_arr;
array<uint32_t,3>first_prem_arr;
ska::bytell_hash_set<string_view>email_set;
void makeIndex()
{
    st_counter.reset();
    time_t ts_joined;
    tm* tm_struct;

    ska::flat_hash_map<uint8_t,uint32_t>prev_country;
    ska::flat_hash_map<uint8_t,uint32_t>prev_fname;
    ska::flat_hash_map<uint16_t,uint32_t>prev_sname;
    ska::flat_hash_map<uint16_t,uint32_t>prev_city;
    ska::flat_hash_map<uint16_t,uint32_t>prev_phonecode;
    array<uint32_t,3>prev_prem{0};
    array<uint32_t,55>prev_birth_year{0};
    for(uint32_t i=max_id;i>0;i--)
    {
#ifdef DEBUG
        auto&& iref=accounts[i];
#endif
        if(EMPTY!=accounts[i].prem_start)    //PREM
        {
            if(prev_prem[NULL0]!=0)
            {
                accounts[prev_prem[NULL0]].nxt_prem=prev_prem[NULL0]-i;
            }
            else first_prem_arr[NULL0]=i;
            prev_prem[NULL0]=i;
        }
        else // empty
        {
            if(prev_prem[NULL1]!=0)
            {
                accounts[prev_prem[NULL1]].nxt_prem=prev_prem[NULL1]-i;
            }
            else first_prem_arr[NULL1]=i;
            prev_prem[NULL1]=i;
        }
        if(true==accounts[i].prem_now)
        {
            if(prev_prem[PREM_NOW]!=0)
            {
                accounts[prev_prem[PREM_NOW]].nxt_prem_now=prev_prem[PREM_NOW]-i;
            }
            else first_prem_arr[PREM_NOW]=i;
            prev_prem[PREM_NOW]=i;
        }

        if(auto &&it=prev_country.find(accounts[i].country);it!=prev_country.end())     //COUNTRY
        {
            accounts[it->second].nxt_country=it->second-i;
        }
        else first_country_map[accounts[i].country]=i;
        prev_country[accounts[i].country]=i;
        if(auto &&it=prev_country.find(NONEMPTY_8);it!=prev_country.end())
        {
            accounts[it->second].nxt_country_nonempty=it->second-i;
        }
        else first_country_map[NONEMPTY_8]=i;
        if(EMPTY!=accounts[i].country)
        {
            prev_country[NONEMPTY_8]=i;
        }


        if(auto &&it=prev_city.find(accounts[i].city);it!=prev_city.end())     //CITY
        {
            accounts[it->second].nxt_city=it->second-i;
        }
        else first_city_map[accounts[i].city]=i;
        prev_city[accounts[i].city]=i;
        if(auto &&it=prev_city.find(NONEMPTY_16);it!=prev_city.end())
        {
            accounts[it->second].nxt_city_nonempty=it->second-i;
        }
        else first_city_map[NONEMPTY_16]=i;
        if(EMPTY!=accounts[i].city)
        {
            prev_city[NONEMPTY_16]=i;
        }


        if(auto &&it=prev_fname.find(accounts[i].fname);it!=prev_fname.end())     //FNAME
        {
            accounts[it->second].nxt_fname=it->second-i;
        }
        else first_fname_map[accounts[i].fname]=i;
        prev_fname[accounts[i].fname]=i;
        if(auto &&it=prev_fname.find(NONEMPTY_8);it!=prev_fname.end())
        {
            accounts[it->second].nxt_fname_nonempty=it->second-i;
        }
        else first_fname_map[NONEMPTY_8]=i;
        if(EMPTY!=accounts[i].fname)
        {
            prev_fname[NONEMPTY_8]=i;
        }


        if(auto &&it=prev_sname.find(accounts[i].sname);it!=prev_sname.end())     //SNAME
        {
            accounts[it->second].nxt_sname=it->second-i;
        }
        else first_sname_map[accounts[i].sname]=i;
        prev_sname[accounts[i].sname]=i;
        if(auto &&it=prev_sname.find(NONEMPTY_16);it!=prev_sname.end())
        {
            accounts[it->second].nxt_sname_nonempty=it->second-i;
        }
        else first_sname_map[NONEMPTY_16]=i;
        if(EMPTY!=accounts[i].sname)
        {
            prev_sname[NONEMPTY_16]=i;
        }


        if(auto &&it=prev_phonecode.find(accounts[i].phone_code);it!=prev_phonecode.end())     //PHONE CODE
        {
            accounts[it->second].nxt_phone_code=it->second-i;
        }
        else first_phonecode_map[accounts[i].phone_code]=i;
        prev_phonecode[accounts[i].phone_code]=i;
        if(auto &&it=prev_phonecode.find(NONEMPTY_16);it!=prev_phonecode.end())
        {
            accounts[it->second].nxt_phonecode_nonempty=it->second-i;
        }
        else first_phonecode_map[NONEMPTY_16]=i;
        if(EMPTY!=accounts[i].phone_code)
        {
            prev_phonecode[NONEMPTY_16]=i;
        }


        if(UINT8_MAX!=accounts[i].birth_year)    //BIRTH
        {
            if(prev_birth_year[accounts[i].birth_year]!=0)
            {
                accounts[prev_birth_year[accounts[i].birth_year]].nxt_birth_year=prev_birth_year[accounts[i].birth_year]-i;
            }
            else first_birth_arr[accounts[i].birth_year]=i;
            prev_birth_year[accounts[i].birth_year]=i;
        }



        //groups
        ts_joined=accounts[i].joined;
        tm_struct=gmtime(&ts_joined);
        auto &&ref_j=groups.f_joined[tm_struct->tm_year-111];
        ref_j.grp_k_country[accounts[i].country].second++;//1900+year-111 2011-2018
        ref_j.grp_k_sex[accounts[i].sex].second++;

        auto &&ref_s=groups.f_sex[accounts[i].sex];
        ref_s.grp_k_city[accounts[i].city].second++;
        ref_s.grp_k_country[accounts[i].country].second++;


        email_set.emplace(string_view(accounts[i].email.str_p,accounts[i].email.len));


        for(int sex=0;sex<2;sex++)
        {
            vector<Liker> temp;
            for (auto &&it=accounts[i].likers[sex].begin();it!=accounts[i].likers[sex].end();)
            {
                uint64_t total=it->ts;
                uint32_t cnt=1;
                while((++it)!=accounts[i].likers[sex].end()&&it->liker_id==(it-1)->liker_id)
                {
                    cnt++;
                    total+=it->ts;
                }
                temp.emplace_back().liker_id=(it-1)->liker_id;
                temp.back().ts=total/cnt;

            }
            accounts[i].likers[sex]=temp;
        }
    }
    for(auto &&it:groups.f_joined)
    {
        it.grp_k_country.sort();
        it.grp_k_sex.sort();
    }
    for(auto &&it:groups.f_sex)
    {
        it.grp_k_country.sort();
        it.grp_k_city.sort();
    }









    st_counter.elapsedPrint("indexing took: ");
}
void reindex()
{

}
