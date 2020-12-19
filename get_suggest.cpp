#include "util.h"
#include "resp_templates.h"
#include <cstdlib>
void getSuggest(char *req, StrP &resp,uint32_t my_id)
{
    bool query_id=false;
    uint8_t limit=0;
    //   vector<uint32_t> city_vec;
    // vector<uint8_t>country_vec;
    StrP256Max city,country;
    do
    {
        if(limit==0&&'m'==*(req+2))//limit
        {
            if(!charr2Uint8(req+=6,limit))goto lbl_r400;
        }
        else if(false==query_id&&'q'==*req)// query_id
        {
            query_id=true;
            while(*++req!='&'&&' '!=*req)
            {}
        }
        else if('t'==*(req+2))//city
        {
            if(*(req+=5)!='%')goto lbl_r400;
            decodeValue(req,city,buffer);
            // decodeVals2Vec32(req+=5,city_vec,buffer,dic_city);
            //if(0==city.len)goto lbl_r400;
        }
        else if('o'==*(req+1))//country
        {
            if(*(req+=8)!='%')goto lbl_r400;
            decodeValue(req,country,buffer);
            // decodeVals2Vec8(req+=8,country_vec,buffer,dic_country);
            // if(0==country.len)goto lbl_r400;
        }
        else
        {
lbl_r400:
            resp.str_p=r400;
            resp.len=r400_len;
            return;
        }
    }
    while(*req++!=' ');
    StrP r200_end(r200_headers_end_p,0);
    charrCopy(r200_end.str_p,k_accounts.str_p,k_accounts.len);
    r200_end.len+=k_accounts.len;


    bool sex=accounts[my_id].sex;
    // ska::flat_hash_map<uint32_t,vector<float>>likers_mp;
    ska::flat_hash_map<uint32_t,uint64_t>likers_mp;//ID-SIMILARITY

    for(auto &&my_like_it:accounts[my_id].likes)
    {
        auto ref=accounts[my_like_it.likee_id];
        // cout<<my_like_it.likee_id<<": ";
        for (auto &&likee_it:accounts[my_like_it.likee_id].likers[sex])
        {
            if(likely(my_id!=likee_it.liker_id))
            {
                //                if(0!=country.len)
                //                {

                //                    if(auto &&cit=dic_country.find(country.str_p);cit==dic_country.end())
                //                    {
                //                        cout<<country.str_p<<endl;
                //                    }

                //                       // accounts[likee_it.liker_id].country!=)continue;
                //                }
//                if(0!=country.len)
//                {

//                    if(auto&&it=dic_country.find(country.str_p);it!=dic_country.end())
//                    {
//                        if(it->second!=accounts[likee_it.liker_id].country)continue;
//                    }
//                }
//                if(0!=city.len)
//                {
//                    if(auto&&it=dic_city.find(city.str_p);it!=dic_city.end())
//                    {
//                        if(it->second!=accounts[likee_it.liker_id].city)continue;
//                    }
//                }

                //  cout<<"!!"<<my_like_it.ts<<"-"<<likee_it.ts<<"= "<<to_string(__builtin_labs((my_like_it.ts - likee_it.ts)))<<"),";
                likers_mp[likee_it.liker_id]+=__builtin_labs(my_like_it.ts - likee_it.ts);
            }
        }
        //cout<<endl
        //    <<endl;
    }

    // map<uint64_t,uint32_t>sim_mp;//similarity-id
    vector<pair<uint64_t,uint32_t>>sim_vec;//sim-id
    sim_vec.reserve(2048);

    for(auto&&it:likers_mp)
    {
        sim_vec.emplace_back(it.second,it.first);
    }
    sort(sim_vec.begin(),sim_vec.end());
    for(auto &&sim_id_it:sim_vec)
    {

        for(auto&&sim_likes_it:accounts[sim_id_it.second].likes)
        {
            __builtin_prefetch(&accounts[sim_likes_it.likee_id]);
            if(sim_likes_it.likee_id!=my_id)
            {
                if(!limit--)goto lbl_out;
                addKVStr(r200_end,k_email,accounts[sim_likes_it.likee_id].email);
                addKVInt(r200_end,k_id,sim_likes_it.likee_id);
                addKVStr(r200_end,k_fname,dic_fname_strings[accounts[sim_likes_it.likee_id].fname].str_p,dic_fname_strings[accounts[sim_likes_it.likee_id].fname].len);
                addKVStr(r200_end,k_sname,dic_sname_strings[accounts[sim_likes_it.likee_id].sname].str_p,dic_sname_strings[accounts[sim_likes_it.likee_id].sname].len);
                charrCopy(r200_end.str_p,k_status_arr[accounts[sim_likes_it.likee_id].status].str_p,k_status_arr[accounts[sim_likes_it.likee_id].status].len);
                r200_end.len+=k_status_arr[accounts[sim_likes_it.likee_id].status].len;
                jsonClose(r200_end);


            }
        }


    }

lbl_out:
    *(r200_end.str_p-1)=']';
    *r200_end.str_p='}';
    r200_end.len+=1;
    resp.str_p=r200;
    resp.len=r200_headers_len+r200_end.len;
    memset(r200_contlen_p,' ',4);
    to_chars(r200_contlen_p,r200_contlen_p+4,r200_end.len);

}
