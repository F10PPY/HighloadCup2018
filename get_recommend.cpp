#include "util.h"
#include "resp_templates.h"

void getRecommend(char *req, StrP &resp,uint32_t id)
{
    bool query_id=false;
    uint8_t limit=0;
    StrP256Max city,country;

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
        else if(city.len==0&&'i'==*(req+1))//city
        {
             decodeValue(req+=5,city,buffer);
             if(0==city.len)goto lbl_r400;
        }
        else if(country.len==0&&'o'==*(req+1))//country
        {
             decodeValue(req+=5,country,buffer);
             if(0==country.len)goto lbl_r400;
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
       StrP r200_end(r200_headers_end_p,0);
    charrCopy(r200_end.str_p,k_accounts.str_p,k_accounts.len);
    r200_end.len+=k_accounts.len;




    *(r200_end.str_p-1)=']';
    *r200_end.str_p='}';
    r200_end.len+=1;
    resp.str_p=r200;
    resp.len=r200_headers_len+r200_end.len;
    memset(r200_contlen_p,' ',4);
    to_chars(r200_contlen_p,r200_contlen_p+4,r200_end.len);

}
