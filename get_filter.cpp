#include "util.h"
#include "resp_templates.h"
#include "chunked_bm_vec.h"
#ifdef DEBUG
std::map<string,uint32_t>filter_param_count;
#endif
struct IStatus
{
    set<uint32_t>id_set;
};
struct ISex
{
    // set<uint32_t>id_set;//
    array<IStatus,3>status;
};
struct //IFilter
{
    array<ISex,2>sex;
    // array<IStatus,3>status;//

}root;

enum precedence{BY_INTERESTS_CONTAINS,BY_SNAME_EQ,BY_CITY_EQ,BY_FNAME_EQ,BY_PHONE_CODE,BY_COUNTRY_EQ,BY_CITY_ANY,BY_BIRTH_YEAR,BY_COUNTRY_NULL1,BY_CITY_NULL1,BY_BIRTH_LT,BY_BIRTH_GT,BY_PREMIUM_NULL1,BY_PREMIUM_NULL0,
                BY_STATUS_NEQ,BY_FNAME_ANY,BY_FNAME_NULL1,BY_SNAME_STARTS,BY_SNAME_NULL1,BY_PHONE_NULL1,BY_PHONE_NULL0,
                BY_INTERESTS_ANY,BY_LIKES_CONTAINS,BY_EMAIL_DOMAIN,BY_EMAIL_LT,BY_EMAIL_GT,
                BY_COUNTRY_NULL0,BY_CITY_NULL0,BY_FNAME_NULL0,BY_SNAME_NULL0,BY_STATUS_EQ,BY_SEX,BY_PREMIUM_NOW};

uint32_t ChunkedBmVec::chunk_sz;
static thread_local ChunkedAggregator chunked_agg;
void getFilter(char *req, StrP &resp)noexcept
{

    // root.sex[MALE].id_set.begin();
    //  root.sex[MALE].status[SINGLE].id_set.begin();
    //   root.status[SINGLE].id_set.begin();
//  ChunkedAggregator chunked_agg;
    //bitset <90> inter_bs;
    struct {
        bool got=false;
        StrP256Max val;
        uint8_t domain;
    }email;
    buffer.reset();

    uint8_t limit=0;
    struct
    {
        bool got=false;
        bool val;
    }sex;
    struct{
        bool got=false;
        uint8_t val;
    }country,status;
    struct{
        bool got=false;
        uint32_t val;
    }birth,phone,sname;
    struct{
        bool got=false;
        vector<uint32_t> vec;
    }city;
    struct{
        bool got=false;
        vector<uint8_t> vec;
    }fname;
    struct{
        bool got=false;
    }premium;
    vector<uint8_t> interests_vec;
    bool query_id=false;
    vector <uint32_t> likes;
    StrP256Max temp_str,sname_prefix_str;
    set<uint32_t> prefix_set;
    set<uint32_t>fname_any_set,city_any_set;
    vector<uint8_t>filters2add;
    filters2add.reserve(8);
    vector<uint8_t>filters2check,combined2check;
    filters2check.reserve(5);
    combined2check.reserve(5);

    bm::aggregator<bm::bvector<> > reverse_inters_agg;
    bm::bvector<> bv_res;
    bm::bvector<>::enumerator en;
    //ChunkedVector ch_vec;

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
        else if(!sex.got&&!memcmp(req,"sex_eq=",6))//sex
        {
            if('f'==*(req+=7)){
                sex.val=FEMALE;
            }
            else  if('m'==*(req)){sex.val=MALE;}
            else goto lbl_r400;
            req++;
            combined2check.emplace_back(BY_SEX);
            filters2add.emplace_back(K_SEX);
            sex.got=true;
        }
        else if(!country.got&&'o'==*(req+1))//country
        {
            if(!memcmp(req+=8,"eq=",3))
            {
                decodeValue(req+=3,temp_str,buffer);
                auto&&it=dic_country.find(temp_str.str_p);
                if (dic_country.end()!=it)
                {
                    country.val=it->second;
                    combined2check.emplace_back(BY_COUNTRY_EQ);
                    filters2add.emplace_back(K_COUNTRY);
                }
                else goto lbl_r400;
            }
            else if(!memcmp(req,"null=",5))
            {
                if(isdigit(*(req+=5)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_COUNTRY_NULL0);
                        filters2add.emplace_back(K_COUNTRY);
                    }
                    else  combined2check.emplace_back(BY_COUNTRY_NULL1);

                }
                else goto lbl_r400;
            }
            else goto lbl_r400;
            country.got=true;
        }
        else if(!city.got&&'y'==*(req+3))//city
        {
            if(!memcmp(req+=5,"eq=",3))
            {
                decodeVals2Vec32(req+=3,city.vec,buffer,dic_city);
                if(city.vec.empty())
                {
lbl_r200_empty:
                    resp.str_p=r200_empty_acc;
                    resp.len=r200_empty_acc_len;
#if defined (DEBUG) &&!defined (MT)
                    st_counter.addTsMcs("r200");
#endif
                    return;
                }
                combined2check.emplace_back(BY_CITY_EQ);
                filters2add.emplace_back(K_CITY);
            }
            else if(!memcmp(req,"any=",4))
            {
                decodeVals2Vec32(req+=4,city.vec,buffer,dic_city);
                if(city.vec.empty())goto lbl_r200_empty;
                //                for(auto&&it:city.vec)
                //                {
                //                    city_any_set.emplace(first_city_map[it]);
                //                }
                //                if(!city_any_set.empty())
                //                {
                //                    combined2check.emplace_back(BY_CITY_ANY);
                //                    filters2add.emplace_back(K_CITY);
                //                }
                combined2check.emplace_back(BY_CITY_ANY);
                filters2add.emplace_back(K_CITY);
            }
            else
            {
                if(isdigit(*(req+=5)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_CITY_NULL0);
                        filters2add.emplace_back(K_CITY);
                    }
                    else combined2check.emplace_back(BY_CITY_NULL1);
                }
                else goto lbl_r400;
            }
            city.got=true;

        }
        else if(!status.got&&'t'==*(req+1))//status
        {
            if('e'==*(req+=7))
            {
                filters2check.emplace_back(BY_STATUS_EQ);
            }
            else if('n'==*req)
            {
                filters2check.emplace_back(BY_STATUS_NEQ);
                req++;
            }
            else goto lbl_r400;
            switch (*(req+=8))
            {
            case '7':
                status.val=ENGAGED;
                req+=31;
                break;
            case '1':
                status.val=SINGLE;
                req+=43;
                break;
            default://'2'
                status.val=COMPLICATED;
                req+=50;
            }
            filters2add.emplace_back(K_STATUS);
            status.got=true;
        }
        else if(interests_vec.empty()&&'i'==*(req))//interests
        {
            if(*(req+=10)=='c')//contains
            {
                combined2check.emplace_back(BY_INTERESTS_CONTAINS);
                req+=9;
            }
            else if(*req=='a')//any
            {
                combined2check.emplace_back(BY_INTERESTS_ANY);
                req+=4;
            }
            else goto lbl_r400;
            if(!decodeVals2Vec8(req,interests_vec,buffer,dic_interest))goto lbl_r200_empty;
            //            inters_to_match=inters_bs_vec[*interests_vec.begin()];
            //            for(auto &&it=interests_vec.begin()+1;it!=interests_vec.end();it++)
            //            {
            //                inters_to_match&=inters_bs_vec[*it];
            //            }
        }
        else if(!birth.got&&'b'==*req)//birth
        {
            switch (*(req+=6))
            {
            case 'l':
                if(!charr2Uint32(req+=3, birth.val))goto lbl_r400;
                filters2check.emplace_back(BY_BIRTH_LT);
                break;
            case 'g':
                if(!charr2Uint32(req+=3, birth.val))goto lbl_r400;
                filters2check.emplace_back(BY_BIRTH_GT);
                break;
            case 'y':
                if(!charr2Uint32(req+=5, birth.val))goto lbl_r400;
                birth.val-=1950;
                combined2check.emplace_back(BY_BIRTH_YEAR);
                break;
            }
            filters2add.emplace_back(K_BIRTH);
            birth.got=true;
        }
        else if(likes.empty()&&'k'==*(req+2))//likes_contains
        {
            if(!readLikes(req+=15,likes))goto lbl_r400;
            filters2check.emplace_back(BY_LIKES_CONTAINS);
        }
        else if(!email.got&&'e'==*req)//email
        {
            if(!memcmp(req+=6,"lt=",3))
            {
                filters2check.emplace_back(BY_EMAIL_LT);
                readValue(req+=3,email.val,buffer);
            }
            else if(!memcmp(req,"gt=",3))
            {
                filters2check.emplace_back(BY_EMAIL_GT);
                readValue(req+=3,email.val,buffer);
            }
            else if(!memcmp(req,"domain=",7))
            {
                readValue(req+=7,email.val,buffer);
                auto&&it=dic_domain.find(email.val.str_p);
                if (dic_domain.end()!=it)
                {
                    email.domain=it->second;
                    filters2check.emplace_back(BY_EMAIL_DOMAIN);
                }
                else goto lbl_r200_empty;
            }
            else goto lbl_r400;
            email.got=true;
        }
        else if(!premium.got&&'r'==*(req+1))//premium
        {
            if('o'==*(req+=9)){
                filters2check.emplace_back(BY_PREMIUM_NOW);
                filters2add.emplace_back(K_PREMIUM);
                req+=4;
            }
            else if ('u'==*req)
            {
                //                if(!charr2Uint8(req+=4, premium.val))goto lbl_r400;
                if(isdigit(*(req+=4)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_PREMIUM_NULL0);
                        filters2add.emplace_back(K_PREMIUM);
                    }
                    else filters2check.emplace_back(BY_PREMIUM_NULL1);
                }
                else goto lbl_r400;
            }
            else goto lbl_r400;

            premium.got=true;
        }
        else if(!sname.got&&!strncmp("sn",req,2))//sname
        {
            switch (*(req+=6))
            {
            case 'e':
            {
                decodeValue(req+=3,temp_str,buffer);
                auto&&it=dic_sname.find(temp_str.str_p);
                if (dic_sname.end()!=it)
                {
                    sname.val=it.value();
                    filters2check.emplace_back(BY_SNAME_EQ);
                    filters2add.emplace_back(K_SNAME);
                }
                else goto lbl_r200_empty;
            }
                break;
            case 's':
            {
                decodeValue(req+=7,sname_prefix_str,buffer);
                auto prefix_range =  dic_sname.equal_prefix_range(sname_prefix_str.str_p);
                for(auto it = prefix_range.first; it != prefix_range.second; ++it)
                {
                    prefix_set.emplace(first_sname_map[*it]);
                }
                if(!prefix_set.empty())
                {
                    filters2check.emplace_back(BY_SNAME_STARTS);
                    filters2add.emplace_back(K_SNAME);
                }
                else goto lbl_r200_empty;
            }
                break;
            default:
                if(isdigit(*(req+=5)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_SNAME_NULL0);
                        filters2add.emplace_back(K_SNAME);
                    }
                    else filters2check.emplace_back(BY_SNAME_NULL1);
                }
                else goto lbl_r400;

            }
            sname.got=true;
        }
        else if(!fname.got&&'f'==*req)//fname
        {
            switch (*(req+=6))
            {
            case 'e':
                if(!decodeVals2Vec8(req+=3,fname.vec,buffer,dic_fname))goto lbl_r200_empty;
                combined2check.emplace_back(BY_FNAME_EQ);
                filters2add.emplace_back(K_FNAME);
                break;
            case 'a':
                if(!decodeVals2Vec8(req+=4,fname.vec,buffer,dic_fname))goto lbl_r200_empty;
                for(auto&&it:fname.vec)
                {
                    fname_any_set.emplace(first_fname_map[it]);
                }
                if(!fname_any_set.empty())
                {
                    combined2check.emplace_back(BY_FNAME_ANY);
                    filters2add.emplace_back(K_FNAME);
                }
                break;
            default:
                if(isdigit(*(req+=5)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_FNAME_NULL0);
                        filters2add.emplace_back(K_FNAME);
                    }
                    else combined2check.emplace_back(BY_FNAME_NULL1);
                }
                else goto lbl_r400;
            }
            fname.got=true;
        }
        else if(!phone.got&&'h'==*(req+1))//phone
        {
            switch (*(req+=6))
            {
            case 'c':
                if(!charr2Uint32(req+=5, phone.val))goto lbl_r400;
                filters2check.emplace_back(BY_PHONE_CODE);
                filters2add.emplace_back(K_PHONE);
                break;
            default:
                if(isdigit(*(req+=5)))
                {
                    if('0'==*req++)
                    {
                        filters2check.emplace_back(BY_PHONE_NULL0);
                        filters2add.emplace_back(K_PHONE);
                    }
                    else  filters2check.emplace_back(BY_PHONE_NULL1);
                }
                else goto lbl_r400;
            }
            phone.got=true;
        }
        else
        {
lbl_r400:
            resp.str_p=r400;
            resp.len=r400_len;
#if defined (DEBUG) &&!defined (MT)
            st_counter.addTsMcs("r400");
#endif
            return;
        }
    }
    while(*req++!=' ');
    st_counter.reset();
    sort(filters2check.rbegin(),filters2check.rend());
    sort(combined2check.begin(),combined2check.end());
#if defined (DEBUG) &&!defined (MT)
    string params_str;
    for(auto &&it=combined2check.begin();it!=combined2check.end();it++)
    {
        switch (*it)
        {
        case BY_BIRTH_YEAR:
            params_str+=("birth_year,");          break;
        case BY_BIRTH_LT:
            params_str+=("birth_lt,");           break;
        case BY_BIRTH_GT:
            params_str+=("birth_gt,");           break;
        case BY_SEX:
            params_str+=("sex_eq,");            break;
        case BY_COUNTRY_EQ:
            params_str+=("country_eq,");         break;
        case BY_COUNTRY_NULL0:
            params_str+=("country0,");           break;
        case BY_COUNTRY_NULL1:
            params_str+=("country1,");           break;
        case BY_CITY_EQ:
            params_str+=("city_eq,");            break;
        case BY_CITY_ANY:
            params_str+=("city_any,");           break;
        case BY_CITY_NULL0:
            params_str+=("city0,");            break;
        case BY_CITY_NULL1:
            params_str+=("city1,");            break;
        case BY_STATUS_EQ:
            params_str+=("status_eq,");            break;
        case BY_STATUS_NEQ:
            params_str+=("status_neq,");            break;
        case BY_INTERESTS_CONTAINS:
            params_str+=("inter_cont,");            break;
        case BY_INTERESTS_ANY:
            params_str+=("inter_any,");            break;
        case BY_EMAIL_GT:
            params_str+=("email_gt,");            break;
        case BY_EMAIL_LT:
            params_str+=("email_lt,");            break;
        case BY_EMAIL_DOMAIN:
            params_str+=("email_domain,");            break;
        case BY_SNAME_EQ:
            params_str+=("sname_eq,");            break;
        case BY_SNAME_STARTS:
            params_str+=("sname_starts,");            break;
        case BY_SNAME_NULL1:
            params_str+=("sname1,");            break;
        case BY_SNAME_NULL0:
            params_str+=("sname0,");            break;
        case BY_FNAME_EQ:
            params_str+=("fname_eq,");            break;
        case BY_FNAME_ANY:
            params_str+=("fname_any,");            break;
        case BY_FNAME_NULL1:
            params_str+=("fname1,");            break;
        case BY_FNAME_NULL0:
            params_str+=("fname0,");            break;
        case BY_PREMIUM_NOW:
            params_str+=("prem_now,");            break;
        case BY_PREMIUM_NULL1:
            params_str+=("prem1,");            break;
        case BY_PREMIUM_NULL0:
            params_str+=("prem0,");            break;
        case BY_PHONE_CODE:
            params_str+=("phone_code,");            break;
        case BY_PHONE_NULL1:
            params_str+=("phone1,");            break;
        case BY_PHONE_NULL0:
            params_str+=("phone0,");            break;
        }
    }
    params_str+=" <COMB ";
    for(auto &&it=filters2check.rbegin();it!=filters2check.rend();it++)
    {
        switch (*it)
        {
        case BY_BIRTH_YEAR:
            params_str+=("birth_year,");          break;
        case BY_BIRTH_LT:
            params_str+=("birth_lt,");           break;
        case BY_BIRTH_GT:
            params_str+=("birth_gt,");           break;
        case BY_SEX:
            params_str+=("sex_eq,");            break;
        case BY_COUNTRY_EQ:
            params_str+=("country_eq,");         break;
        case BY_COUNTRY_NULL0:
            params_str+=("country0,");           break;
        case BY_COUNTRY_NULL1:
            params_str+=("country1,");           break;
        case BY_CITY_EQ:
            params_str+=("city_eq,");            break;
        case BY_CITY_ANY:
            params_str+=("city_any,");           break;
        case BY_CITY_NULL0:
            params_str+=("city0,");            break;
        case BY_CITY_NULL1:
            params_str+=("city1,");            break;
        case BY_STATUS_EQ:
            params_str+=("status_eq,");            break;
        case BY_STATUS_NEQ:
            params_str+=("status_neq,");            break;
        case BY_INTERESTS_CONTAINS:
            params_str+=("inter_cont,");            break;
        case BY_INTERESTS_ANY:
            params_str+=("inter_any,");            break;
        case BY_EMAIL_GT:
            params_str+=("email_gt,");            break;
        case BY_EMAIL_LT:
            params_str+=("email_lt,");            break;
        case BY_EMAIL_DOMAIN:
            params_str+=("email_domain,");            break;
        case BY_SNAME_EQ:
            params_str+=("sname_eq,");            break;
        case BY_SNAME_STARTS:
            params_str+=("sname_starts,");            break;
        case BY_SNAME_NULL1:
            params_str+=("sname1,");            break;
        case BY_SNAME_NULL0:
            params_str+=("sname0,");            break;
        case BY_FNAME_EQ:
            params_str+=("fname_eq,");            break;
        case BY_FNAME_ANY:
            params_str+=("fname_any,");            break;
        case BY_FNAME_NULL1:
            params_str+=("fname1,");            break;
        case BY_FNAME_NULL0:
            params_str+=("fname0,");            break;
        case BY_PREMIUM_NOW:
            params_str+=("prem_now,");            break;
        case BY_PREMIUM_NULL1:
            params_str+=("prem1,");            break;
        case BY_PREMIUM_NULL0:
            params_str+=("prem0,");            break;
        case BY_PHONE_CODE:
            params_str+=("phone_code,");            break;
        case BY_PHONE_NULL1:
            params_str+=("phone1,");            break;
        case BY_PHONE_NULL0:
            params_str+=("phone0,");            break;
        }
    }
#endif


    StrP r200_end(r200_headers_end_p,0);
    charrCopy(r200_end.str_p,k_accounts.str_p,k_accounts.len);
    r200_end.len+=k_accounts.len;

    uint32_t id=max_id;
    uint8_t index_field=nxt_fullscan;


    if(!combined2check.empty())
    {
        chunked_agg.reset();
        index_field=nxt_combined;
        for( auto &&it:combined2check)
        {
            switch (it)
            {
            case BY_BIRTH_YEAR:
                chunked_agg.add(chunked_birth_year_vec[birth.val]);
                break;
            case BY_SEX:
                chunked_agg.add(chunked_sex_vec[sex.val]);
                break;
            case BY_INTERESTS_CONTAINS:
            {
               // auto && ref=chunked_agg.addAggStep(AGGR_AND);
                for(auto&&it:interests_vec)
                {
                    //ref.addAggSrc(&chunked_inters_vec[it]);
                    chunked_agg.add(chunked_inters_vec[it]);
                }
            }
                break;
            case BY_COUNTRY_EQ:
                chunked_agg.add(chunked_country_vec[country.val]);
                break;
            case BY_COUNTRY_NULL1:
                chunked_agg.add(chunked_country_vec[EMPTY]);
                break;
            case BY_FNAME_EQ:
                chunked_agg.add(chunked_fname_vec[fname.vec[0]]);
                break;
            case BY_FNAME_ANY:
            {
                auto && ref=chunked_agg.addAggStep(AGGR_OR);
                for(auto&&it:fname.vec)
                {
                    ref.addAggSrc(&chunked_fname_vec[it]);
                }
            }
                break;
            case BY_FNAME_NULL1:
                chunked_agg.add(chunked_fname_vec[EMPTY]);
                break;
//            case BY_SNAME_EQ:
//                chunked_agg.add(chunked_sname_vec[fname.vec[0]]);
//                break;
//            case BY_SNAME_STARTS:
//            {
//                auto && ref=chunked_agg.addAggStep(AGGR_OR);
//                auto prefix_range =  dic_sname.equal_prefix_range(sname_prefix_str.str_p);
//                for(auto it = prefix_range.first; it != prefix_range.second; ++it)
//                {
//                    //  prefix_set.emplace(first_sname_map[*it]);
//                    ref.addAggSrc(&chunked_sname_vec[*it]);
//                }
//            }
//                break;
//            case BY_SNAME_NULL1:
//                chunked_agg.add(chunked_sname_vec[EMPTY]);
//                break;
            case BY_CITY_EQ:
                chunked_agg.add(chunked_city_vec[city.vec[0]]);
                break;
            case BY_CITY_NULL1:
                chunked_agg.add(chunked_city_vec[EMPTY]);
                break;

            case BY_CITY_ANY:
            {
                auto && ref=chunked_agg.addAggStep(AGGR_OR);
                for(auto&&it:city.vec)
                {
                    ref.addAggSrc(&chunked_city_vec[it]);
                }
            }
                break;
            case BY_INTERESTS_ANY:
            {
                auto && ref=chunked_agg.addAggStep(AGGR_OR);
                for(auto&&it:interests_vec)
                {
                    ref.addAggSrc(&chunked_inters_vec[it]);
                }
            }
                break;
            }
        }
        chunked_agg.findFirst();
        if(!chunked_agg.getNextBit(id))
            goto lbl_out;


    }
    else if(!filters2check.empty())
    {
        switch (filters2check.back())
        {
        //        case BY_COUNTRY_EQ:
        //            id=first_country_map[country.val];
        //            index_field=nxt_country;
        //            filters2check.pop_back();
        //            break;
        //        case BY_COUNTRY_NULL1:
        //            id=first_country_map[EMPTY];
        //            index_field=nxt_country;
        //            filters2check.pop_back();
        //            break;
        case BY_LIKES_CONTAINS:
        //{
//            vector<int>a,b,c;

//            set_intersection(a.begin(),a.end(),b.begin(),b.end(),c.begin());




//            vector<Liker>temp;
//            for(auto&&it:likes)
//            {
//                set_intersection(accounts[it].likers.begin(),accounts[it].likers.end(),accounts[it].likers.begin(),accounts[it].likers.end(),back_inserter(temp));
//            }


//            sort(likes.begin(),likes.end());
//        }
            break;
        case BY_COUNTRY_NULL0:
            id=first_country_map[NONEMPTY_8];
            index_field=nxt_country_nonempty;
            filters2check.pop_back();
            break;
            //        case BY_CITY_EQ:
            //            id=first_city_map[static_cast<uint16_t>(city.vec.front())];
            //            index_field=nxt_city;
            //            filters2check.pop_back();
            //            break;
            //        case BY_CITY_ANY:
            //            id=*city_any_set.rbegin();
            //            index_field=nxt_city_any;
            //            filters2check.pop_back();
            //            break;
            //        case BY_CITY_NULL1:
            //            id=first_city_map[EMPTY];
            //            index_field=nxt_city;
            //            filters2check.pop_back();
            //            break;
        case BY_CITY_NULL0:
            id=first_city_map[NONEMPTY_16];
            index_field=nxt_city_nonempty;
            filters2check.pop_back();
            break;
        case BY_PHONE_CODE:
            id=first_phonecode_map[static_cast<uint16_t>(phone.val)];
            index_field=nxt_phonecode;
            filters2check.pop_back();
            break;
        case BY_PHONE_NULL1:
            id=first_phonecode_map[NONEMPTY_16];
            index_field=nxt_phonecode;
            filters2check.pop_back();
            break;
        case BY_PHONE_NULL0:
            id=first_phonecode_map[NONEMPTY_16];
            index_field=nxt_phonecode_nonempty;
            filters2check.pop_back();
            break;
        case BY_PREMIUM_NOW:
            id=first_prem_arr[PREM_NOW];
            index_field=nxt_prem_now;
            filters2check.pop_back();
            break;
        case BY_PREMIUM_NULL1:
            id=first_prem_arr[NULL1];
            index_field=nxt_prem;
            filters2check.pop_back();
            break;
        case BY_PREMIUM_NULL0:
            id=first_prem_arr[NULL0];
            index_field=nxt_prem;
            filters2check.pop_back();
            break;
            //        case BY_FNAME_EQ:
            //            id=first_fname_map[fname.vec.front()];
            //            index_field=nxt_fname;
            //            filters2check.pop_back();
            //            break;
            //        case BY_FNAME_ANY:
            //            id=*fname_any_set.rbegin();
            //            index_field=nxt_fname_any;
            //            filters2check.pop_back();
            //            break;
            //        case BY_FNAME_NULL1:
            //            id=first_fname_map[EMPTY];
            //            index_field=nxt_fname;
            //            filters2check.pop_back();
            //            break;
        case BY_FNAME_NULL0:
            id=first_fname_map[NONEMPTY_8];
            index_field=nxt_fname_nonempty;
            filters2check.pop_back();
            break;
        case BY_SNAME_EQ:
            id=first_sname_map[static_cast<uint16_t>(sname.val)];
            index_field=nxt_sname;
            filters2check.pop_back();
            break;
        case BY_SNAME_NULL1:
            id=first_sname_map[EMPTY];
            index_field=nxt_sname;
            filters2check.pop_back();
            break;
        case BY_SNAME_NULL0:
            id=first_sname_map[NONEMPTY_16];
            index_field=nxt_sname_nonempty;
            filters2check.pop_back();
            break;
        case BY_SNAME_STARTS:
            id=*prefix_set.rbegin();
            index_field=nxt_sname_starts;
            filters2check.pop_back();
            break;
        case BY_BIRTH_YEAR:
            id=first_birth_arr[birth.val];
            index_field=nxt_birth_year;
            filters2check.pop_back();
            break;
            //        case BY_INTERESTS_CONTAINS:
            //            auto && ref=ch_vec.addAggStep(AGGR_AND);
            //            for(auto&&it:interests_vec)
            //            {
            //                ref.addAggSrc(&chunked_inters_vec[it]);
            //            }

            //            //            for(auto&&it:interests_vec)
            //            //            {
            //            //                chagg.add(chunked_inters_vec[it]);
            //            //            }
            //            //            chagg.getFirstOnBit(id);
            //            //            if(!chagg.getNextOnBit(id))goto lbl_out;
            //            index_field=nxt_combined;
            //            filters2check.pop_back();
            //            break;

        }
    }


    if (id!=0)
    {
        while(0!=limit)
        {
            __builtin_prefetch (&accounts[id]);
            //  auto &&ref=accounts[i];
            for(auto &&it=filters2check.rbegin();it!=filters2check.rend();it++)
            {
                switch (*it)
                {
                case BY_BIRTH_YEAR:
                    // cout<<accounts[i].birth_year<<"  "<<birth.val<<endl;
                    if(accounts[id].birth_year!=birth.val)goto nxt_id;
                    break;
                case BY_BIRTH_LT:
                    if(accounts[id].birth_ts>birth.val)goto nxt_id;
                    break;
                case BY_BIRTH_GT:
                    if(accounts[id].birth_ts<birth.val)goto nxt_id;
                    break;
                case BY_SEX:
                    if(accounts[id].sex!=sex.val)goto nxt_id;
                    break;
                case BY_COUNTRY_EQ:
                    if(country.val!=accounts[id].country)goto nxt_id;
                    break;
                case BY_COUNTRY_NULL0:
                    if(accounts[id].country==EMPTY)goto nxt_id;
                    break;
                case BY_COUNTRY_NULL1:
                    if(accounts[id].country!=EMPTY)goto nxt_id;
                    break;
                case BY_CITY_EQ:
                    if(*city.vec.begin()!=accounts[id].city)goto nxt_id;
                    break;
                case BY_CITY_ANY:
                {
                    auto &&it=city.vec.begin();
                    while(it!=city.vec.end())
                    {
                        if(accounts[id].city==*it)break;
                        it++;
                    }
                    if(it==city.vec.end())goto nxt_id;
                }
                    break;
                case BY_CITY_NULL0:
                    if(accounts[id].city==EMPTY)goto nxt_id;
                    break;
                case BY_CITY_NULL1:
                    if(accounts[id].city!=EMPTY)goto nxt_id;
                    break;
                case BY_STATUS_EQ:
                    if(accounts[id].status!=status.val)goto nxt_id;
                    break;
                case BY_STATUS_NEQ:
                    if(accounts[id].status==status.val)goto nxt_id;
                    break;
                case BY_INTERESTS_CONTAINS:
                {
                    //                                        for(auto &&it:interests_vec)
                    //                                        {
                    //                                            if(!inters_bs_vec[it][id])goto nxt_id;
                    //                                        }

                    break;

                }
                case BY_INTERESTS_ANY:
                {
                    auto &&it=interests_vec.begin();
                    while(it!=interests_vec.end())
                    {
                        if(accounts[id].interests_bs[*it]==true)break;
                        it++;
                    }
                    if(it==interests_vec.end())
                        goto nxt_id;
                }
                    break;
                case BY_EMAIL_GT:
                    if(0<strncmp(email.val.str_p,accounts[id].email.str_p,accounts[id].email.len))goto nxt_id;
                    break;
                case BY_EMAIL_LT:
                    if(0>strncmp(email.val.str_p,accounts[id].email.str_p,accounts[id].email.len))goto nxt_id;
                    break;
                case BY_EMAIL_DOMAIN:
                    if(email.domain!=accounts[id].email_domain)goto nxt_id;
                    break;
                case BY_SNAME_EQ:
                    if(sname.val!=accounts[id].sname)goto nxt_id;
                    break;
                case BY_SNAME_STARTS:
                    if(0!=strncmp(dic_sname_strings[accounts[id].sname].str_p,sname_prefix_str.str_p,sname_prefix_str.len))goto nxt_id;
                    break;
                case BY_SNAME_NULL1:
                    if(accounts[id].sname!=EMPTY)goto nxt_id;
                    break;
                case BY_SNAME_NULL0:
                    if(accounts[id].sname==EMPTY)goto nxt_id;
                    break;
                case BY_FNAME_EQ:
                    if(*fname.vec.begin()!=accounts[id].fname)goto nxt_id;
                    break;
                case BY_FNAME_ANY:
                {
                    auto &&it=fname.vec.begin();
                    while(it!=fname.vec.end())
                    {
                        if(*it==accounts[id].fname)break;
                        it++;
                    }
                    if(it==fname.vec.end())goto nxt_id;
                }
                    break;
                case BY_FNAME_NULL1:
                    if(accounts[id].fname!=EMPTY)goto nxt_id;
                    break;
                case BY_FNAME_NULL0:
                    if(accounts[id].fname==EMPTY)goto nxt_id;
                    break;
                case BY_PREMIUM_NOW:
                    if(!accounts[id].prem_now)goto nxt_id;
                    break;
                case BY_PREMIUM_NULL1:
                    if(accounts[id].prem_start!=EMPTY)goto nxt_id;
                    break;
                case BY_PREMIUM_NULL0:
                    if(accounts[id].prem_start==EMPTY)goto nxt_id;
                    break;
                case BY_PHONE_CODE:
                    if(accounts[id].phone_code!=phone.val)goto nxt_id;
                    break;
                case BY_PHONE_NULL1:
                    if(accounts[id].phone_code!=0)goto nxt_id;
                    break;
                case BY_PHONE_NULL0:
                    if(accounts[id].phone_code==0)goto nxt_id;
                    break;
                }


            }

            addKVStr(r200_end,k_email,accounts[id].email);
            addKVInt(r200_end,k_id,id);

            for(auto&&n:filters2add)
            {
                switch (n)
                {
                case K_COUNTRY:
                    addKVStr(r200_end,k_country,dic_country_strings[accounts[id].country].str_p,dic_country_strings[accounts[id].country].len);
                    break;
                case K_CITY:
                    addKVStr(r200_end,k_city,dic_city_strings[accounts[id].city].str_p,dic_city_strings[accounts[id].city].len);
                    break;
                case K_PREMIUM:
                    addKVPrem(r200_end,accounts[id].prem_finish,accounts[id].prem_start);
                    break;
                case K_BIRTH:
                    addKVInt(r200_end,k_birth,accounts[id].birth_ts);
                    break;
                case K_SEX:
                    charrCopy(r200_end.str_p,k_sex_arr[accounts[id].sex].str_p,k_sex_arr[accounts[id].sex].len);
                    r200_end.len+=k_sex_f.len;
                    break;
                case K_PHONE:
                    addKVStr(r200_end,k_phone,accounts[id].phone);
                    break;
                case K_FNAME:
                    addKVStr(r200_end,k_fname,dic_fname_strings[accounts[id].fname].str_p,dic_fname_strings[accounts[id].fname].len);
                    break;
                case K_SNAME:
                    addKVStr(r200_end,k_sname,dic_sname_strings[accounts[id].sname].str_p,dic_sname_strings[accounts[id].sname].len);
                    break;
                case K_STATUS:
                    charrCopy(r200_end.str_p,k_status_arr[accounts[id].status].str_p,k_status_arr[accounts[id].status].len);
                    r200_end.len+=k_status_arr[accounts[id].status].len;
                    break;
                }

            }
            jsonClose(r200_end);

            limit--;

nxt_id:
            switch (index_field)
            {
            case nxt_birth_year:
                if(0==accounts[id].nxt_birth_year)goto lbl_out;
                id-=accounts[id].nxt_birth_year;
                break;
            case nxt_country:
                if(0==accounts[id].nxt_country)goto lbl_out;
                id-=accounts[id].nxt_country;
                break;
            case nxt_country_nonempty:
                if(0==accounts[id].nxt_country_nonempty)goto lbl_out;
                id-=accounts[id].nxt_country_nonempty;
                break;
            case nxt_city:
                if(0==accounts[id].nxt_city)goto lbl_out;
                id-=accounts[id].nxt_city;
                break;
            case nxt_city_any:
                city_any_set.erase(std::prev(city_any_set.end()));
                if(0!=accounts[id].nxt_city)
                    city_any_set.emplace_hint(city_any_set.end(),id-accounts[id].nxt_city);
                if(!city_any_set.empty())id=*city_any_set.rbegin();
                else goto lbl_out;
                break;
            case nxt_city_nonempty:
                if(0==accounts[id].nxt_city_nonempty)goto lbl_out;
                id-=accounts[id].nxt_city_nonempty;
                break;
            case nxt_phonecode:
                if(0!=accounts[id].nxt_phone_code)id-=accounts[id].nxt_phone_code;
                else goto lbl_out;
                break;
            case nxt_phonecode_nonempty:
                if(0!=accounts[id].nxt_phonecode_nonempty)id-=accounts[id].nxt_phonecode_nonempty;
                else goto lbl_out;
                break;
            case nxt_prem_now:
                if(0!=accounts[id].nxt_prem_now)id-=accounts[id].nxt_prem_now;
                else goto lbl_out;
                break;
            case nxt_prem:
                if(0!=accounts[id].nxt_prem)id-=accounts[id].nxt_prem;
                else goto lbl_out;
                break;
            case nxt_fname:
                if(0!=accounts[id].nxt_fname)id-=accounts[id].nxt_fname;
                else goto lbl_out;
                break;
            case nxt_fname_nonempty:
                if(0!=accounts[id].nxt_fname_nonempty)id-=accounts[id].nxt_fname_nonempty;
                else goto lbl_out;
                break;
            case nxt_fname_any:
                fname_any_set.erase(std::prev(fname_any_set.end()));
                if(0!=accounts[id].nxt_fname)
                    fname_any_set.emplace_hint(fname_any_set.end(),id-accounts[id].nxt_fname);
                if(!fname_any_set.empty())id=*fname_any_set.rbegin();
                else goto lbl_out;
                break;
            case nxt_sname:
                if(0!=accounts[id].nxt_sname)id-=accounts[id].nxt_sname;
                else goto lbl_out;
                break;
            case nxt_sname_nonempty:
                if(0!=accounts[id].nxt_sname_nonempty)id-=accounts[id].nxt_sname_nonempty;
                else goto lbl_out;
                break;
            case nxt_sname_starts:
                prefix_set.erase(std::prev(prefix_set.end()));
                if(0!=accounts[id].nxt_sname)
                    prefix_set.emplace_hint(prefix_set.end(),id-accounts[id].nxt_sname);
                if(!prefix_set.empty())id=*prefix_set.rbegin();
                else goto lbl_out;
                break;
            case nxt_fullscan:
                if(0==id--)goto lbl_out;
                break;

                /// case nxt_interests:
            case nxt_combined:
            default:

                // if(!ch_vec.getNextOnBit(id)) goto lbl_out;
                if(!chunked_agg.getNextBit(id)) goto lbl_out;
                break;
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
#if defined (DEBUG) &&!defined (MT)
    st_counter.addTsMcs(params_str);
#endif
#ifdef DEBUG
    if(resp.len>8192)puts("resp.len more than 8192!");
#endif
    return;
}

//    // for(auto&&  it:accounts)
//    for (auto  &&it = acc_map.rbegin(); it != acc_map.rend(); ++it)
//        //     for (auto  &&it = acc_map.begin(); it != acc_map.end(); ++it)
//    {
//        if(0==limit)break;
//        if(sex!=NONE)
//        {
//            if(it->second.sex!=sex)continue;
//        }
//        if(country.pred!=NONE){
//            if(it->second.country!=EMPTY8)
//            {
//                if(country.pred==EQ)
//                {
//                    if(country.val!=it->second.country)continue;
//                    //   if(0!=strncmp(country.val.str_p,it->second.country.str_p,it->second.country.len))continue;
//                }
//                else// if(sname.pred==NUL)
//                {
//                    if(country.null==NUL1)continue;
//                }
//            }
//            else if(country.pred==NUL)
//            {
//                if(country.null==NUL0)continue;
//            }
//            else continue; //EQ but not exists
//        }

//        if(city.pred!=NONE)
//        {
//            if(it->second.city.len)
//            {
//                if(city.pred==EQ)
//                {
//                    if(0!=strncmp(city.vec.begin()->str_p,it->second.city.str_p,it->second.city.len))continue;
//                }
//                else if (city.pred==ANY)
//                {
//                    auto &&i=city.vec.begin();
//                    while(i!=city.vec.end())
//                    {
//                        if(0==strncmp(i->str_p,it->second.city.str_p,it->second.city.len))break;
//                        i++;
//                    }
//                    if(i==city.vec.end())continue;
//                }
//                else// if(fname.pred==NUL)
//                {
//                    if(city.null==NUL1)continue;
//                }
//            }
//            else if(city.pred==NUL)
//            {
//                if(city.null==NUL0)continue;
//            }
//            else continue;
//        }

//        if(status.pred!=NONE){
//            if(status.pred==EQ){
//                if(it->second.status!=status.val)continue;
//            }
//            else// if(status.pred==NEQ){
//                if(it->second.status==status.val)continue;
//        }


//        if(!interests.vec.empty())
//        {
//            auto &&i=interests.vec.begin();
//            if(interests.pred==CONTAINS){

//                while(i!=interests.vec.end())
//                {
//                    if(it->second.interests.end()==it->second.interests.find( i->str_p))break;

//                    i++;
//                }
//                if(i!=interests.vec.end())continue;
//            }
//            else{ //ANY
//                while(i!=interests.vec.end())
//                {
//                    if(it->second.interests.end()!=it->second.interests.find( i->str_p))break;
//                    i++;
//                }
//                if(i==interests.vec.end())continue;
//            }
//        }

//        if(birth.pred!=NONE){
//            if(birth.pred==YEAR){
//                if(it->second.birth_year!=birth.val)continue;
//            }
//            else if(birth.pred==LT){
//                if(it->second.birth_ts>birth.val)continue;
//            }
//            else// if(birth.pred==GT){
//                if(it->second.birth_ts<birth.val)continue;
//        }


//        if(!likes.empty()){//contains
//            auto &&i=likes.begin();
//            while(i!=likes.end())
//            {
//                //  if(it->second.likes.end()==it->second.likes.find(*i))break;
//                i++;
//            }
//            if(i!=likes.end())continue;
//            if(!includes(likes.begin().base(),likes.end().base(),it->second.likes.begin().base(),it->second.likes.end().base(),compLike))continue;
//        }

//        if(email.pred!=NONE)
//        {
//            if(email.pred==DOMAIN){
//                if(email.domain!=it->second.email_domain)continue;
//            }
//            else if(email.pred==LT)
//            {
//                if(0>strncmp(email.val.str_p,it->second.email.str_p,it->second.email.len))continue;
//            }
//            else// if(email.pred==GT){
//                if(0<strncmp(email.val.str_p,it->second.email.str_p,it->second.email.len))continue;
//        }

//        if(premium.pred!=NONE){
//            if(premium.pred==NOW){
//                if(!it->second.prem_now)continue;
//            }
//            else//NUL
//            {
//                if(it->second.prem_start!=EMPTY32)
//                {
//                    if(NUL1==premium.val)continue;
//                }
//                else if(NUL0==premium.val)continue;
//            }
//        }


//        if(sname.pred!=NONE){
//            if(it->second.sname.len)
//            {
//                if(sname.pred==EQ)
//                {
//                    if(0!=strncmp(sname.val.str_p,it->second.sname.str_p,it->second.sname.len))continue;
//                }
//                else if (sname.pred==STARTS)
//                {
//                    if(0!=strncmp(it->second.sname.str_p,sname.val.str_p,sname.val.len))continue;
//                }
//                else// if(sname.pred==NUL)
//                {
//                    if(sname.null==NUL1)continue;
//                }
//            }
//            else if(sname.pred==NUL)
//            {
//                if(sname.null==NUL0)continue;
//            }
//            else continue;
//        }

//        if(fname.pred!=NONE){
//            if(it->second.fname.len)
//            {
//                if(fname.pred==EQ)
//                {
//                    if(0!=strncmp(fname.vec.begin()->str_p,it->second.fname.str_p,it->second.fname.len))continue;
//                }
//                else if (fname.pred==ANY)
//                {
//                    auto &&i=fname.vec.begin();
//                    while(i!=fname.vec.end())
//                    {
//                        if(0==strncmp(i->str_p,it->second.fname.str_p,it->second.fname.len))break;
//                        i++;
//                    }
//                    if(i==fname.vec.end())continue;
//                }
//                else// if(fname.pred==NUL)
//                {
//                    if(fname.null==NUL1)continue;
//                }
//            }
//            else if(fname.pred==NUL)
//            {
//                if(fname.null==NUL0)continue;
//            }
//            else continue;
//        }

//        if(phone.pred!=NONE){
//            if(phone.pred==CODE){
//                if(it->second.phone_code!=phone.val)continue;
//            }
//            else// if(phone.pred==NUL)
//            {
//                // if(true==phone.val&&0!=it->second.phone_code)continue;//if field not set acc->code !=0
//                //  else  if(0==it->second.phone_code)continue;
//                if(it->second.phone_code)
//                {
//                    if(NUL1==phone.val)continue;
//                }
//                else if(NUL0==phone.val)continue;
//            }
//        }


//        limit--;
//        addKVStr(r200_end,k_email,it->second.email);
//        addKVInt(r200_end,k_id,it->first);
//        if(sex!=NONE)
//        {
//            charrCopy(r200_end.str_p,k_sex_arr[it->second.sex].str_p,k_sex_arr[it->second.sex].len);
//            r200_end.len+=k_sex_f.len;
//        }
//        if(country.pred==EQ||(country.pred==NUL&&country.null==NUL0))
//        {
//            //  addKVStr(r200_end,k_country,it->second.country);
//            // cout.write(dic_country_strings[it->second.country].str_p,dic_country_strings[it->second.country].len)<<endl;
//            addKVStr(r200_end,k_country,dic_country_strings[it->second.country].str_p,dic_country_strings[it->second.country].len);
//        }
//        //        if(city.pred!=NONE&&!(city.pred==NUL&&city.null==true))
//        //        {  addKVStr(r200_end,k_city,it->second.city);
//        //        }
//        if(status.pred!=NONE){
//            charrCopy(r200_end.str_p,k_status_arr[it->second.status].str_p,k_status_arr[it->second.status].len);
//            r200_end.len+=k_status_arr[it->second.status].len;
//        }
//        if(birth.pred!=NONE){
//            addKVInt(r200_end,k_birth,it->second.birth_ts);
//        }
//        if(premium.pred==NOW||(premium.pred==NUL&&premium.val==false))
//        {   addKVPrem(r200_end,it->second.prem_finish,it->second.prem_start);

//        }
//        //        if(sname.pred!=NONE&&!(sname.pred==NUL&&sname.null==true))
//        //        {   addKVStr(r200_end,k_sname,it->second.sname);
//        //        }
//        //        if(fname.pred!=NONE&&!(fname.pred==NUL&&fname.null==true))
//        //        {  addKVStr(r200_end,k_fname,it->second.fname);
//        //        }

//        if(phone.pred==CODE||(phone.pred==NUL&&phone.val==false)){
//            addKVStr(r200_end,k_phone,it->second.phone);
//        }


//        *(r200_end.str_p-1)='}';
//        *r200_end.str_p++=',';
//        r200_end.len++;



//    }
//    *(r200_end.str_p-1)=']';
//    *r200_end.str_p='}';
//    r200_end.len+=1;
//    resp.str_p=r200;
//    resp.len=r200_headers_len+r200_end.len;
//    memset(r200_contlen_p,' ',4);
//    to_chars(r200_contlen_p,r200_contlen_p+4,r200_end.len);
//    return;
//}
