#include "common.h"
#include "util.h"
#include "resp_templates.h"
#include <signal.h>
#include "post.h"
thread_local BufArr buffer(4096); //thread_local
//static size_t n_filter=0,n_group=0,n_recom=0,n_sugg=0,n_new=0,n_likes=0,n_update=0;


void getFilter(char *req, StrP& resp)noexcept;
void getGroup(char *req, StrP &resp)noexcept;
void getRecommend(char *req, StrP &resp,uint32_t id)noexcept;
void getSuggest(char *req, StrP &resp,uint32_t id)noexcept;
void postNew(char *req,int fd)noexcept;
void postId(char *req,int fd,uint32_t id)noexcept;
void postLikes(char *req,int fd)noexcept;

void reqGet(char *req,int fd)noexcept
{
    uint32_t id;
    StrP resp;
#if defined (DEBUG) &&!defined (MT)
   // ntb::Counter::resetStatic();

#endif
    if(!memcmp(&req[14],"filter/?",8))
    {
#if defined (DEBUG) &&!defined (MT)

#endif
        getFilter(&req[22],resp);
    }
    else if(!memcmp(&req[14],"group/?",7))
    {
#if defined (DEBUG) &&!defined (MT)
      //  gg_ksex_fjoined.reset();
#endif
        getGroup(&req[21],resp);
    }
#ifdef DEBUG
    else if(!memcmp(req+5,"bp",2))
    {
        cout<<"breakpoint"<<endl;
    }
#ifndef MT
    else if(!memcmp(req+5,"clear",5))
    {
        st_counter.printAllCollectionsStats();
        st_counter.clearAllCollections();
        sendto(fd,r202,r202_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
    }
#endif
#endif
    else
    {
        if(charr2Uint32(req+=14,id)&&id<=max_id)
        {
            if(*++req=='r')
            {
                getRecommend(req+11,resp,id);
            }
            else{
                getSuggest(req+9,resp,id);

            }
        }
        else
        {
            sendto(fd,r404,r404_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
            return;
        }
    }
    sendto(fd,resp.str_p,resp.len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);
    //  return;

}
void reqPost(char *req,char *body,int fd)noexcept
{
    uint32_t id;
    if(charr2Uint32(req+=15,id))
    {
        if(max_id>=id) postId(body,fd,id);
        else sendto(fd, r404, r404_len, MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE, nullptr, 0);
   }
    else if(!memcmp(req,"likes/?",7))
    {
        postLikes(body, fd);

    }
    else if(!memcmp(req,"new/?",5))
    {
        postNew(body,fd);

    }
    else
    {
        sendto(fd,r404,r404_len,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTWAIT|MSG_DONTROUTE,nullptr,0);

    }
    //   return;

    // puts(req);
}
