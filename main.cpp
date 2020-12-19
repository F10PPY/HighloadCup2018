#include "util.h"
#include <sys/uio.h>
#include <syscall.h>
#include "resp_templates.h"
#include "post.h"


//constexpr const int MAX_EVENTS=2048;
//constexpr const int  RECV_SZ=6122;
ntb::Counter st_counter;
ska::flat_hash_map <string,uint8_t> post_mp;
void loadData();

void reqGet(char *recv_buff,int fd)noexcept;
void reqPost(char *req,char *body,int fd)noexcept;
[[noreturn]]void worker(int wrk_epoll,int tid)noexcept;
#ifdef DEBUG
char *last_req=new char[512];
#endif
//void tests();
//void tests_bm();
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
//void setSockopt(int fd,int optname)
//{
//    int enable = 1;
//    if(-1==setsockopt(fd, SOL_SOCKET,optname, &enable, sizeof(int)))
//    {
//        cout<<"err setsockopt"<<endl;
//    }
//}


int main()
{

//    memUsage();
//    bm::bvector b;
//    memUsage();
//    b.set_bit(120000000);
//    memUsage();
    initSig();
    initPostMap();
    loadData();
    // memUsage();
    makeIndex();




    int master_sock=socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    int enable = 1;
    if(-1==setsockopt(master_sock, SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT, &enable, sizeof(int)))
    {
        cout<<"err setsockopt"<<endl;
    }
    if(-1==setsockopt(master_sock, IPPROTO_TCP,TCP_NODELAY, &enable, sizeof(int)))
    {
        cout<<"err setsockopt"<<endl;
    }
    if(-1==setsockopt(master_sock, IPPROTO_TCP,TCP_QUICKACK, &enable, sizeof(int)))
    {
        cout<<"err setsockopt"<<endl;
    }

    //     int sendbuff;
    //     socklen_t optlen;

    //         // Get buffer size
    //     optlen = sizeof(sendbuff);
    //     getsockopt(master_sock, SOL_SOCKET, SO_RCVBUF, &sendbuff, &optlen);


    //     printf("sets the send buffer to %d\n", sendbuff);
    //     sendbuff = 4096;
    //     setsockopt(master_sock, SOL_SOCKET, SO_RCVBUF, &sendbuff, sizeof(int));
    //printf("sets the send buffer to %d\n", sendbuff);
    //     // Get buffer size
    //     optlen = sizeof(sendbuff);
    //     getsockopt(master_sock, SOL_SOCKET, SO_RCVBUF, &sendbuff, &optlen);

    //printf("sets the send buffer to %d\n", sendbuff);


    struct sockaddr_in s;
    s.sin_family=AF_INET;

#ifdef DEBUG_BUILD
    s.sin_port=htons(1080);
#else
    s.sin_port=htons(80);
#endif
    inet_pton(AF_INET, "0.0.0.0", &s.sin_addr);
    if(-1==bind(master_sock,reinterpret_cast<struct sockaddr*>(&s),sizeof(s)))
    {
        cout<<"bind err"<<strerror(errno)<<endl;
        return 0;
    }
    if(-1==listen(master_sock,2048))
    {
        cout<<"tcp listen error"<<endl;
        return 0;
    }
    epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=master_sock;




    cout<<"threads: "<<NUM_THREADS<<" ";
    int t_arr[NUM_THREADS];
    thread t_pool[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        t_arr[i]=epoll_create1(0);
        t_pool[i] = thread(worker,t_arr[i],i);
        t_pool[i].detach();
    }
    memUsage();
    uint8_t tn=0;
    while (true)
    {
        event.data.fd=accept4(master_sock,nullptr,nullptr,O_NONBLOCK);
        if(event.data.fd!=-1)
        {
            if(-1==setsockopt(event.data.fd, IPPROTO_TCP,TCP_NODELAY, &enable, sizeof(int)))
            {
                cout<<"err setsockopt"<<endl;
            }
            if(-1==setsockopt(event.data.fd, IPPROTO_TCP,TCP_QUICKACK, &enable, sizeof(int)))
            {
                cout<<"err setsockopt"<<endl;
            }
            epoll_ctl(t_arr[tn],EPOLL_CTL_ADD,event.data.fd,&event);
            if(++tn==NUM_THREADS)tn=0;
        }
    }
    return EXIT_SUCCESS;
}
