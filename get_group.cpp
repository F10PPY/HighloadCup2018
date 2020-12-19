#include "resp_templates.h"
#include "get_group.h"
enum {EMPTY8=UINT8_MAX,ASCEND=1,DESCEND=2};
#ifdef DEBUG
std::map<string,uint32_t>group_param_count;
#endif

grp groups;

void getGroup(char *req, StrP &resp)
{
    buffer.reset();
    uint8_t limit=0;
    bool ksex=false;
    bool kstatus=false;
    bool kinterests=false;
    bool kcountry=false;
    bool kcity=false;
    bool kflag=false;
    bool query_id=false;
    uint8_t order=EMPTY,fsex=EMPTY8,fstatus=NONE;
    uint32_t fbirth=NONE,fjoined=NONE,flikes=NONE;
    StrP256Max fcity,fcountry,finterests;
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
        else if(!kflag&&'k'==*req)//keys
        {
            kflag=true;
            req+=4;
            do
            {
                req++;
                if(*(req-1)=='%')req+=2;
                if(*req=='i')//interests
                {
                    kinterests=true;
                    req+=9;
                }
                else if (*(req+1)=='e')//sex
                {
                    ksex=true;
                    req+=3;
                }
                else if(*(req+1)=='o')//country
                {
                    kcountry=true;
                    req+=7;
                }
                else if(*(req+1)=='i')//city
                {
                    kcity=true;
                    req+=4;
                }
                else if(*(req+1)=='t')//status
                {
                    kstatus=true;
                    req+=6;
                }
                else goto lbl_r400;
            }
            while(*req!='&'&&' '!=*req);

        }
        else if(!memcmp(req,"order=",6))//order
        {
            if(*(req+=6)=='-')
            {
                order=DESCEND;
                req+=2;
            }
            else if(*(req)=='1')
            {
                order=ASCEND;
                req++;
            }
        }
        else if(fbirth==NONE&&'b'==*req)//birth
        {

            if(!charr2Uint32(req+=6, fbirth))goto lbl_r400;
            fbirth-=1900;
        }
        else if(fsex==EMPTY8&&'e'==*(req+1))//sex
        {
            if('f'==*(req+=4)){
                fsex=FEMALE;
            }
            else  if('m'==*(req)){fsex=MALE;}
            else goto lbl_r400;
            req++;
        }
        else if(fjoined==EMPTY&&'j'==*req)//joined
        {

            if(!charr2Uint32(req+=7,fjoined))goto lbl_r400;
            fjoined-=2011;
        }
        else if(flikes==NONE&&'l'==*req)//likes
        {

            if(!charr2Uint32(req+=6, flikes))goto lbl_r400;
        }
        else if(fcity.len==0&&'c'==*req)//city
        {

            decodeValue(req+=5,fcity,buffer);
        }
        //        else if(country.len==0&&'o'==*req)//country
        //        {

        //            decodeValue(req+=8,country,buffer);
        //        }
        else if(finterests.len==0&&'i'==*req)//interests
        {

            decodeValue(req+=10,finterests,buffer);
        }
        else if(fstatus==NONE&&'t'==*(req+1))//status
        {
            switch (*(req+=12))
            {
            case '7':
                fstatus=ENGAGED;
                req+=31;
                break;
            case '1':
                fstatus=SINGLE;
                req+=43;
                break;
            default://'2'
                fstatus=COMPLICATED;
                req+=50;
            }
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
//#ifdef DEBUG
//    //temp->
//    string temp_keys_str;
//    if(ksex){
//        temp_keys_str+=("ksex,");
//    }
//    if(kstatus){
//        temp_keys_str+=("kstatus,");
//    }
//    if(kinterests){
//        temp_keys_str+=("kinterests,");
//    }
//    if(kcity){
//        temp_keys_str+=("kcity,");
//    }
//    if(kcountry){
//        temp_keys_str+=("kcountry,");
//    }
//    temp_keys_str+=" fields: ";
//    if(sex){
//        temp_keys_str+=("sex,");
//    }
//    if(status){
//        temp_keys_str+=("status,");
//    }
//    if(birth){
//        temp_keys_str+=("birth,");
//    }
//    if(joined){
//        temp_keys_str+=("joined,");
//    }
//    if(likes){
//        temp_keys_str+=("likes,");
//    }
//    if(city.len){
//        temp_keys_str+=("city,");
//    }
//    if(country.len){
//        temp_keys_str+=("country,");
//    }
//    if(interests.len){
//        temp_keys_str+=("interests,");
//    }
//    auto &&itf=group_param_count.find(temp_keys_str);
//    if(group_param_count.end()==itf){
//        group_param_count.emplace(temp_keys_str,1);
//    }
//    else {
//        itf->second++;
//    }
//#endif

    StrP r200_end(r200_headers_end_p,0);
    charrCopy(r200_end.str_p,k_groups.str_p,k_groups.len);
    r200_end.len+=k_groups.len;

    if(fjoined!=EMPTY)
    {
        if(kcountry==true)
        {
            if(ASCEND==order)
            {
                for(auto &&it:groups.f_joined[fjoined].grp_k_country)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it.second);
                    if(it.first) addKVStr(r200_end,k_country,dic_country_strings[it.first].str_p,dic_country_strings[it.first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
            else
            {
                for(auto &&it=groups.f_joined[fjoined].grp_k_country.rbegin();it!=groups.f_joined[fjoined].grp_k_country.rend();it++)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it->second);
                    if(it->first) addKVStr(r200_end,k_country,dic_country_strings[it->first].str_p,dic_country_strings[it->first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
        }
        else if(ksex)
        {
            if(ASCEND==order)
            {
                for(auto &&it:groups.f_joined[fjoined].grp_k_sex)
                {
                    addKVInt(r200_end,k_count,it.second);
                    charrCopy(r200_end.str_p,k_sex_arr[it.first].str_p,k_sex_arr[it.first].len);
                    r200_end.len+=k_sex_f.len;
                    jsonClose(r200_end);
                }
            }
            else
            {
               for(auto &&it=groups.f_joined[fjoined].grp_k_sex.rbegin();it!=groups.f_joined[fjoined].grp_k_sex.rend();it++)
                {
                    addKVInt(r200_end,k_count,it->second);
                    charrCopy(r200_end.str_p,k_sex_arr[it->first].str_p,k_sex_arr[it->first].len);
                    r200_end.len+=k_sex_f.len;
                    jsonClose(r200_end);
                }
            }
        }
    }
    else if (fsex!=EMPTY8)
    {
        if(kcountry==true)
        {
            if(ASCEND==order)
            {
                for(auto &&it:groups.f_sex[fsex].grp_k_country)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it.second);
                    if(it.first) addKVStr(r200_end,k_country,dic_country_strings[it.first].str_p,dic_country_strings[it.first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
            else
            {
                for(auto &&it=groups.f_sex[fsex].grp_k_country.rbegin();it!=groups.f_sex[fsex].grp_k_country.rend();it++)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it->second);
                    if(it->first) addKVStr(r200_end,k_country,dic_country_strings[it->first].str_p,dic_country_strings[it->first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
        }
        else if(kcity)
        {
            if(ASCEND==order)
            {
                for(auto &&it:groups.f_sex[fsex].grp_k_city)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it.second);
                    if(it.first) addKVStr(r200_end,k_city,dic_city_strings[it.first].str_p,dic_city_strings[it.first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
            else
            {
                for(auto &&it=groups.f_sex[fsex].grp_k_city.rbegin();it!=groups.f_sex[fsex].grp_k_city.rend();it++)
                {
                    if(!limit)break;
                    addKVInt(r200_end,k_count,it->second);
                    if(it->first) addKVStr(r200_end,k_city,dic_city_strings[it->first].str_p,dic_city_strings[it->first].len);
                    jsonClose(r200_end);

                    limit--;
                }
            }
        }
    }



    *(r200_end.str_p-1)=']';
    *r200_end.str_p='}';
    r200_end.len+=1;
    resp.str_p=r200;
    resp.len=r200_headers_len+r200_end.len;
    memset(r200_contlen_p,' ',4);
    to_chars(r200_contlen_p,r200_contlen_p+4,r200_end.len);

}
