#include "util.h"
#include "resp_templates.h"
#include "post.h"

//static uint32_t last=0;
void postNew(char *req,int fd)noexcept
{
    //if(doc.ParseInsitu(req).HasParseError())
    if(*req!='{')
    {
        sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
        return;
    }
    Document doc;
    doc.ParseInsitu(req);
    Account* ref;
    uint32_t id;
    // uint8_t nmemb=0;

    Value::MemberIterator itr=doc.FindMember("id");
    if(itr!= doc.MemberEnd())
    {
        sendto(fd,r201,r201_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
        id=itr->value.GetUint();
        ref=&accounts[id];
        __builtin_prefetch(ref);
        //        if(0!=accounts[id].email.len)//check if already exists
    }
    else
    {
        sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
        return;
    }
    string dummy;
    itr=doc.FindMember("email");
    if(itr!= doc.MemberEnd())//||!it->value.IsString())
    {
        accounts[id].email.len=itr->value.GetStringLength();
        accounts[id].email.str_p=new char[accounts[id].email.len];
        memcpy(accounts[id].email.str_p, itr->value.GetString(),accounts[id].email.len);

        const char* pos=strchr(accounts[id].email.str_p,'@');
        ptrdiff_t pdiff=&accounts[id].email.str_p[accounts[id].email.len]-pos;
        dummy.assign(pos+1,pdiff-1);                  //DOMAIN
        accounts[id].email_domain=dic_domain[dummy];

        email_set.emplace(string_view(accounts[id].email.str_p, accounts[id].email.len));

    }
    itr=doc.FindMember("phone");
    if(itr!= doc.MemberEnd())
    {
        accounts[id].phone.len=itr->value.GetStringLength();
        accounts[id].phone.str_p=new char[accounts[id].phone.len];
        memcpy(accounts[id].phone.str_p, itr->value.GetString(),accounts[id].phone.len);
        accounts[id].phone_code= charrGetUint16(&accounts[id].phone.str_p[2]);
    }
    itr=doc.FindMember("country");                  //COUNTRY
    if ( itr!= doc.MemberEnd())
    {
        accounts[id].country=dic_country[itr->value.GetString()];
    }
    itr=doc.FindMember("city");                  //CITY
    if ( itr!= doc.MemberEnd())
    {
        accounts[id].city=dic_city[itr->value.GetString()];
    }
    itr=doc.FindMember("fname");                  //FNAME
    if ( itr!= doc.MemberEnd())
    {
        accounts[id].fname=dic_fname[itr->value.GetString()];
    }
    itr=doc.FindMember("sname");                  //SNAME
    if ( itr!= doc.MemberEnd())
    {
        accounts[id].sname=dic_fname[itr->value.GetString()];
    }
    itr=doc.FindMember("phone");                  //PHONE
    if ( itr!= doc.MemberEnd())
    {
        ref->phone.len=itr->value.GetStringLength();
        ref->phone.str_p=new char[ref->phone.len];
        memcpy(ref->phone.str_p, itr->value.GetString(), ref->phone.len);
        ref->phone_code=charrGetUint16(&ref->phone.str_p[2]);
    }
    itr=doc.FindMember("interests");                  //INTERESRS
    if ( itr!= doc.MemberEnd())
    {
        for (SizeType j = 0; j <  itr->value.GetArray().Size(); j++)
        {
           // dummy=itr->value[j].GetString();
            //auto &&um_it=dic_interest.find(dummy);
//            if (dic_interest.end()==um_it)
//            {
//                dic_interest_strings.emplace_back(dummy.data());
//                dic_interest.emplace(dummy,dic_interest_val);

//                ref->interests_bs[dic_interest_val]=true;
//                inters_bs_vec.emplace_back()[id]=true;
//                    // inters_bs_vec[dic_interest_val][i]=true;//setting bitset here
//                dic_interest_val++;
//            }
//            else
            {
                   inters_bs_vec[dic_interest[itr->value[j].GetString()]][id]=true;
                    //ref->interests_bs[um_it->second]=true;
            }

        }
    }
    itr=doc.FindMember("premium");                  //PREMIUM
    if ( itr!= doc.MemberEnd())
    {
        if(itr->value.HasMember("start"))
        {
            ref->prem_start=itr->value["start"].GetUint();
        }

        if(itr->value.HasMember("finish"))
        {
            ref->prem_finish=itr->value["finish"].GetUint();
            if(ref->prem_finish>options_ts)ref->prem_now=true;
        }
    }

    time_t timeBorn;
    tm* tm_struct;
    ref->birth_ts=doc["birth"].GetUint();
    timeBorn=ref->birth_ts;
    tm_struct=gmtime(&timeBorn);
    ref->birth_year=tm_struct->tm_year-50;                  //BIRTH

    ref->joined=doc["joined"].GetUint();                  //JOINED

    if(!strcmp("заняты",doc["status"].GetString()))ref->status=ENGAGED;                  //STATUS
    else if (!strcmp("свободны",doc["status"].GetString()))ref->status=SINGLE;
    else ref->status=COMPLICATED;

    if('m'==doc["sex"].GetString()[0])ref->sex=MALE;                  //SEX
    else ref->sex=FEMALE;








    //    if(last>id)cout<<"random:/"<<endl;
    //    last=id;
    if(max_id<id)max_id=id;

    return;
}
