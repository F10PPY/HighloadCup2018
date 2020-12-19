#include "common.h"
#include "util.h"

void decodeValue(char *&req,StrP256Max &to,BufArr &arr)
{
    while (*req != ' '&&*req!='&')
    {
        if (*req == '%')
        {
            if (req[1] && req[2])
            {
                arr.ch() = from_hex(req[1]) << 4 | from_hex(req[2]);
                req += 2;
            }
        } else if (*req == '+')
        {
            arr.ch() = ' ';
        } else {
            arr.ch()= *req;
        }
        req++;
        //if (*++req == ' ')return true;
    }
    to.len=arr.getLen();
    to.str_p=arr.endLn();
}

bool decodeVals2Vec8(char *&req, vector<uint8_t> &vec,BufArr& arr,ska::flat_hash_map<string,uint8_t> &dic)
{
    do
    {
        if (*req == '%')
        {
            if ('2'==req[1] && 'C'==req[2])//comma
            {
                auto&&it=dic.find(arr.endLn());
                 if (dic.end()!=it)
                {
                    vec.emplace_back(it->second);
                }
                else return false;
                req += 2;
            }
            else if (req[1] && req[2])
            {
                arr.ch()= from_hex(req[1]) << 4 | from_hex(req[2]);
                req += 2;
            }
        } else if (*req == '+') {
            arr.ch() = ' ';
        } else {
            arr.ch() = *req;
        }
    }
    while (*++req != ' '&&*req!='&');
    auto&&it=dic.find(arr.endLn());

    if(dic.end()!=it)
    {
        vec.emplace_back(it->second);
        return true;
    }
    return false;
}
void decodeVals2Vec32(char *&req, vector<uint32_t> &vec, BufArr& arr, ska::flat_hash_map<string, uint32_t> &dic)
{
    do
    {
        if (*req == '%')
        {
            if ('2'==req[1] && 'C'==req[2])//comma
            {
                auto&&it=dic.find(arr.endLn());

                if (dic.end()!=it)
                {
                    vec.emplace_back(it->second);
                }
              //  else   return false;
                req += 2;
            }
            else if (req[1] && req[2])
            {
                arr.ch()= from_hex(req[1]) << 4 | from_hex(req[2]);
                req += 2;
            }
        } else if (*req == '+') {
            arr.ch() = ' ';
        } else {
            arr.ch() = *req;
        }
    }
    while (*++req != ' '&&*req!='&');

    auto&&it=dic.find(arr.endLn());

    if(dic.end()!=it)
    {
        vec.emplace_back(it->second);
      //  return true;
    }
  //  return false;

}

void memUsage()
{
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }

    cout << "VM: " << vsize / 1024.0 << "; RSS: " <<  rss * (sysconf(_SC_PAGE_SIZE) / 1024) << endl;
}
