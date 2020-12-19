#include "util.h"
#include "resp_templates.h"
#include "post.h"

void postId(char *req,int fd,uint32_t id)noexcept
{
    //if(doc.ParseInsitu(req).HasParseError())
    if(*req!='{')
    {
        sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
        return;
    }
    Document doc;
    doc.ParseInsitu(req);

    string dummy_str,email_str,sex_str;
    string status_str;
    status_str.reserve(50);
    email_str.reserve(100);
    dummy_str.reserve(100);

    size_t pos;
    vector<uint8_t> fields2change;
    fields2change.reserve(12);
    // ska::flat_hash_map<string,uint8_t>::iterator mit;
    for (Value::ConstMemberIterator &&itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr)
    {
        // mit=post_mp.find(itr->name.GetString());
        // dummy_str=itr->name.GetString();
        switch (post_mp.at(itr->name.GetString()))
        {
        case K_EMAIL:
            email_str=itr->value.GetString();
            if(string::npos==(pos=email_str.find('@')))
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            //            for(uint32_t i=1;i<max_id;i++)
            //            {
            //                if(0==strncmp(accounts[i].email.str_p,email_str.data(),accounts[i].email.len))
            //                {
            //                    sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
            //                    return;
            //                }
            //            }
            if(email_set.find(email_str)!=email_set.end())
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }


            fields2change.emplace_back(K_EMAIL);
            break;
        case K_PREMIUM:
            if(!itr->value.IsObject())
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            fields2change.emplace_back(K_PREMIUM);
            break;
        case K_SEX:
            sex_str=itr->value.GetString();
            if(sex_str!="m"&&sex_str!="f")
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            fields2change.emplace_back(K_SEX);
            break;
        case K_STATUS:
        {
            status_str=itr->value.GetString();//129
            if(0!=status_str.compare(0,2,"за",2)&&0!=status_str.compare(0,2,"вс",2)&&0!=status_str.compare(0,2,"св",2))
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            fields2change.emplace_back(K_STATUS);
        }
            break;
        case K_BIRTH:
            if(!itr->value.IsInt())
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            fields2change.emplace_back(K_BIRTH);
            break;
        case K_JOINED:
            if(!itr->value.IsInt())
            {
                sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
                return;
            }
            fields2change.emplace_back(K_JOINED);
            break;
        case K_LIKES:
            sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
            return;

        default:
            // cout<<post_mp[itr->name.GetString()]<<" "<<itr->name.GetString()<<endl;
            break;
        }
    }
    sendto(fd,r202,r202_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);

    //    Value::MemberIterator  itr=doc.FindMember("email");
    //    if(itr!= doc.MemberEnd())//||!it->value.IsString())
    //    {
    //        accounts[id].email.len=itr->value.GetStringLength();
    //        accounts[id].email.str_p=new char[accounts[id].email.len];
    //        memcpy(accounts[id].email.str_p, itr->value.GetString(),accounts[id].email.len);

    //        const char* pos=strchr(accounts[id].email.str_p,'@');
    //        ptrdiff_t pdiff=&accounts[id].email.str_p[accounts[id].email.len]-pos;
    //        dummy.assign(pos+1,pdiff-1);                  //DOMAIN
    //        accounts[id].email_domain=dic_domain[dummy];
    //    }
    //    itr=doc.FindMember("phone");
    //    if(itr!= doc.MemberEnd())
    //    {
    //        accounts[id].phone.len=itr->value.GetStringLength();
    //        accounts[id].phone.str_p=new char[accounts[id].phone.len];
    //        memcpy(accounts[id].phone.str_p, itr->value.GetString(),accounts[id].phone.len);
    //        accounts[id].phone_code= charrGetUint16(&accounts[id].phone.str_p[2]);
    //    }

    return;
}
