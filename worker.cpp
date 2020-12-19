#include "util.h"
#include "resp_templates.h"
#include <syscall.h>
constexpr const int MAX_EVENTS=1024;
constexpr const int  RECV_SZ=6144;
atomic_uint32_t post_cnt=0;
void reqGet(char *recv_buff,int fd)noexcept;
void reqPost(char *req,char *body,int fd)noexcept;
[[noreturn]]void worker(int wrk_epoll,int tid)noexcept
{
    initRespBuff();

    char *recv_buff=new char[RECV_SZ];
    char* body_p;
    StrP resp;
    epoll_event events[MAX_EVENTS];
    epoll_event event;
    event.events=EPOLLIN;

    int n=0;
    int i=0;
    ssize_t recv_result;
    ssize_t res=0;
    string prev;
    uint32_t cont_len=0;
#if defined (DEBUG) &&!defined (MT)
    uint32_t cnt=0;
#endif
inf_loop:
    {

#ifdef LOCAL
        n=epoll_wait(wrk_epoll,events,MAX_EVENTS,-1);
#else
        n=syscall(SYS_epoll_wait, wrk_epoll, events,MAX_EVENTS, 0);//slower?
        // n=epoll_wait(wrk_epoll,events,MAX_EVENTS,0);
#endif
        for(i=0;i<n;++i)
        {
            if(events[i].events & EPOLLHUP)
            {
                close(events[i].data.fd);
            }
            else if(likely(events[i].events & 0x001))
            {
                recv_result=recvfrom(events[i].data.fd,recv_buff,RECV_SZ,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTROUTE,nullptr,nullptr);

                if(recv_result>0)
                {
#if defined (DEBUG) &&!defined (MT)
                    if((++cnt==28000&&!is_rating)||cnt==177000)
                    {
                        for(auto &&it:filter_param_count)
                        {
                            cout<< it.first<<": "<<it.second<<endl;
                        }
                        st_counter.printAllCollectionsStats();
                    }
                    last_req=recv_buff;
#endif
                    if('G'==*recv_buff)
                    {
                        reqGet(recv_buff,events[i].data.fd);
                    }
                    else  if('P'==*recv_buff)
                    {
                        body_p=strstr(&recv_buff[100],"gth:")+5;
                        charr2Uint32(body_p,cont_len);
                        body_p+=36;//\r\nContent-Type: application/json\r\n\r\n
                        cont_len+=body_p-recv_buff;

                        while(cont_len>recv_result)
                        {
                            //cout<<cont_len<<" received: "<<recv_result<<endl;
                            res=recvfrom(events[i].data.fd,recv_buff+recv_result,RECV_SZ,MSG_NOSIGNAL|MSG_ZEROCOPY|MSG_DONTROUTE|MSG_WAITALL,nullptr,nullptr);
                            if(res>0)
                            {
                                recv_result+=res;
                            }
                            else if(res==-1)
                            {

                                if(errno!=EWOULDBLOCK&&errno!=EAGAIN) cout<<strerror(errno)<<endl;
                            }
                            //                            else
                            //                            {
                            //                                close(events[i].data.fd);
                            //                                goto inf_loop;// WRONG
                            //                            }
                        }
                        recv_buff[recv_result]='\0';
                        reqPost(recv_buff,body_p,events[i].data.fd);
                        if(++post_cnt==90000||(!is_rating&&post_cnt==10000))
                        {
                            makeIndex();
                        }
                    }
#ifdef DEBUG
                    else
                    {write(STDOUT_FILENO,recv_buff,recv_result);
                        fflush(stdout);
                    }
                    prev.assign(recv_buff,recv_result);
#endif
                }
                else close(events[i].data.fd);

            }
        }
        goto inf_loop;
    }
    return;
}
