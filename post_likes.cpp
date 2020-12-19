//#include "util.h"
#include "resp_templates.h"
#include "post.h"

void postLikes(char *req,int fd)noexcept
{
    if(*req!='{')
    {
        sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
        return;
    }
    Document doc;
    doc.ParseInsitu(req);

    Value::ConstValueIterator i;

    for(auto &&it:doc["likes"].GetArray())
    {
         i=&it["liker"];
        if(!i->IsInt()||i->GetUint()>=max_id||!it["ts"].IsInt())
        {
            sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
            return;
        }
        i=&it["likee"];
        if(!i->IsInt()||i->GetUint()>=max_id)
        {
            sendto(fd,r400,r400_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
            return;
        }
    }
    sendto(fd,r202,r202_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
}
