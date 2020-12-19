#include "util.h"
struct bits21X3_64{//2`097`152  1,3kk=9,92Mb 433,333=3,3Mb
    uint32_t x:21;
    uint32_t y:21;
    uint32_t z:21;
}__attribute__((packed));
#include <random>
#include <functional>

char arr[1024];
char arr2[1024];
std::string randomString( size_t length )
{
    auto randchar = []() -> char
    {
            const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[ rand() % max_index ];
};
std::string str(length,0);
std::generate_n( str.begin(), length, randchar );
return str;
}
struct t1
{
    uint32_t birth_ts=0;
    StrP256MaxPk email,city,fname,sname,phone;
    int phone_code:1;
    uint8_t status=0;
    uint8_t email_domain=0;
    uint8_t birth_year=0;
    uint8_t country=NONE;
    bool sex,prem_now=false;
    uint32_t joined=0,prem_start=0,prem_finish=0;
   // t1(){}
    // vector <char*> interests;
    ska::bytell_hash_set <string>interests;
    vector <Like> likes;
    // unordered_map<uint16_t,uint32_t> likes;//id/ts
}__attribute__((packed));
struct t2:public t1
{
   // t2(const t1& obj) : t1(obj) { }
   // t1 hz;
   // uint32_t id;
     ska::bytell_hash_set <string>interests;
     vector <Like> likes;
}__attribute__((packed));
#include <unordered_set>
void tests()
{
    // return 0;

     //    vector <bool>v(1'300'000);
     //    ntb::clReset();
     //    int z = 0;
     //     memUsage();
     //      set<uint32_t> h;
     //    for(uint i=0;i<10000;i++)
     //    {
     //        // v.push_back(i);
     //        h.emplace(i);
     //       // if(v[i]==true)hz++;
     //    }
     //    ntb::clElapsedPrint();
     //    memUsage();
     //        cout<<z<<endl;
     //    // tests();
     //    return 0;

     //    memUsage();
     //    ngvs_map<uint16_t,uint8_t>mp(0);

     //    for(uint8_t i=0;i<90;i++)
     //    {
     //            mp.set(i,rand());
     //    }
     //    mp.emplace(0,2);
     //    mp.emplace(1,4);
     //    mp.emplace(2,6);
     //    mp.emplace(3,0);
     //    mp[0]=2;
     //     mp[1]=4;
     //      mp[2]=6;
     //       mp[3]=0;
     //    mp.sort();
     //            memUsage();
     //    for(auto &&it:mp)
     //    {
     //        cout<<it.second<<endl;
     //    }
     //            for(int i=0;i<100;i++)
     //            {
     //                mp.dec(2);
     //            }




    //  vector<t1> hz1(1'000'000);
     // vector<t1> hz2(1'000'000);
    //  memUsage();
    //  return 0;
//          string dum="asdhesndf";
//          st_counter.reset();
//          ska::flat_hash_map<uint32_t,uint32_t>fm;
//          ska::flat_hash_map<uint32_t,uint32_t>bm;
//          ska::unordered_map<uint32_t,uint32_t>sm;
//         // ska::flat_hash_set<uint32_t> us;
//          std::set<uint32_t>us;
//          char *c=new char[10];
//         // um.emplace(555555,99);
//          sm.emplace(555555,99);
//          fm.emplace(555555,99);
//         // memcpy(p,randomString(10).data(),10);
//          ska::bytell_hash_set<uint32_t> bs;
//        //  std::unordered_set<uint32_t> us;
//          uint32_t nn=0;
//          memUsage();
//          for(int i=0;i<1'300'000;i++)
//          {
//             // bm.emplace(rand(),++nn);
//            //  bm.emplace(randomString(10),++nn);

//             us.emplace(rand());


//          }

////          for(int i=0;i<100'000'000;i++)
////          {
////             // bm.find(555555);
////              us.find(5);
////          }

//          st_counter.elapsedPrint("um");
//          memUsage();
//          cout<<us.size()<<"sz"<<bm.size()<<endl;
//          return ;
      //    char d[]="dsf";
      //    hash<char*> st;
      //    map<char*,int,st>mp;
      //       std::vector<uint32_t> v1(1300000), v2(10000);
      //       std::generate(v1.begin(), v1.end(), rand);
      //       std::generate(v2.begin(), v2.end(), rand);
      //     //  fill(v2.begin(),v2.end(),0);
      //       // sort
      //       std::sort(v1.begin(), v1.end());
      //       std::sort(v2.begin(), v2.end());
      //       std::vector<uint32_t> v;
      //    ntb::clReset();
      //    merge(v1.begin(), v1.end(),v2.begin(), v2.end(), std::back_inserter(v));
      //    ntb::clElapsedPrint();
      //    for(auto &&it=v.begin();it!=v.begin()+30;it++)
      //    {
      //        cout<<*it<<endl;
      //    }
      //    return 0;
      //    for (int i=0;i<100000;i++) {
      //        v.push_back("");
      //        v.back().resize(256);
      //    }
      //    vector<char*> v;
      //    for (int i=0;i<100000;i++) {
      //    v.push_back(new char[256]);
      //    }
      //  memUsage();
      //   return 0;

      //      char src[256]= "235567ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd";
      //      char dest[256]="dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddz";
      //      char *p=&src[0];
      //      uint32_t temp=0;
      //      int t = 0;
      //          ntb::clReset();
      //          for (uint64_t i=0;i<30000000;i++)
      //          {
      //           t+=strtol(src,nullptr,0);
      //         //  charr2Uint32(p,temp);

      //           //   from_chars(p,p+6,temp);//wtf 3x slower than strtol
      //              // cout<<temp<<endl;
      //           t+=temp;
      //           //t+=235567;
      //           p=&src[0];
      //          }
      //          cout<<t<<endl;
      //          ntb::clElapsedPrint();
      //          return 0;

      //  char *src=new char[4096];
      // char *dest=new char[4096];
      //    char* dest_p=dest;
      //    int temp = 0;
      //    int nn=strlen(src);
      //    ntb::clReset();
      //    for (uint64_t i=0;i<300000000;i++) {
      //      memcpy(src,dest_p,256);
      //      //  charrCopy(dest_p,src,256);

      //        dest_p=dest;
      //       // src[0]++;
      //       // dest[0]++;
      //        temp+=memcmp(src,dest,256);

      //    }
      //    cout<<temp<<endl;
      //    ntb::clElapsedPrint();

      // return 0;

      //    vector<bits21X3_64> vec;
      //    memUsage();
      //    //for(int i=0;i<1'300'000;i++){
      //        for(int i=0;i<433'333;i++){
      //        vec.push_back(bits21X3_64{});
      //    }
      //    memUsage();
      //    return 0;

      //        int c = 0;
      //        ntb::clReset();
      //        int len1=strlen(src);
      //        int len2=strlen(dest);
      //        for(int i=0;i<10000000;i++)
      //        {
      //         //   c+=strcmp(src,dest);
      //           // c+=strncmp(src,dest,256);
      //            memset(src,5,256);
      //            memset(dest,5,256);

      //          //  c+=memcmp(src,dest,256);
      //          //  c+=charrCmp(src,256,dest,256);
      //          // if(!(0==c))printf("%i\n",c);
      //         //   src[0]++;
      //           // dest[0]++;

      //        }
      //       cout<<c<<endl;
      //        ntb::clElapsedPrint();
      //        return 0;


      //    char st[128];
      //    string temp;
      //    int x=12343249;
      //    char *p=&st[0];
      //    vector<int> vec;
      //    ntb::clReset();
      //  //  to_chars_result res;
      //    for(int i=0;i<14000000;i++)
      //    {
      ////        temp=to_string(x);
      ////        charrCopy(p,temp.data(),temp.length());
      ////        p=&st[0];
      //       // memset(st,' ',19);
      //        to_chars_result res=to_chars(st,st+19,x);
      //        p=res.ptr;
      //        //p+=19;
      //        p=&st[0];
      //    }
      //    ntb::clElapsedPrint();
      //    cout<<x<<endl;
      //    return 0;
}
