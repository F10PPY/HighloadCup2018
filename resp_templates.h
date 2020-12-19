#ifndef RESP_TEMPLATES_H
#define RESP_TEMPLATES_H
#include "util.h"
extern char r404[];
extern char r400[];
extern thread_local char *r200;
extern char r200_empty_acc[];
extern const uint8_t r404_len;
extern const uint8_t r400_len;
extern const uint8_t r200_empty_acc_len;
extern uint32_t r200_headers_len;
extern thread_local char* r200_contlen_p;
extern thread_local char* r200_headers_end_p;
extern StrP256Max k_email;
extern StrP256Max k_sex_m;
extern StrP256Max k_sex_f;
extern StrP256Max k_sex_arr[2];
extern StrP256Max k_status_single;
extern StrP256Max k_status_engaged;
extern StrP256Max k_status_complicated;
extern StrP256Max k_status_arr[3];
extern StrP256Max k_country;
extern StrP256Max k_city;
extern StrP256Max k_id;
extern StrP256Max k_birth;
extern StrP256Max k_phone;
extern StrP256Max k_fname;
extern StrP256Max k_sname;
extern StrP256Max k_premium;
extern StrP256Max k_accounts,k_groups;
extern StrP256Max k_count;

inline void initRespBuff()
{
    r200=new char[8192];
    strcpy(r200,"HTTP/1.1 200 O\r\nConnection: keep-alive\r\nContent-Type: text/plain; charset=utf-8\r\nContent-length:     \r\n\r\n");
    r200_contlen_p=strstr(r200,"gth:")+5;
    r200_headers_len=strlen(r200);
    r200_headers_end_p=&r200[r200_headers_len];
    return;
}
inline void initRTemplates()//call only after initRespBuff
{
    r200=new char[8192];
    strcpy(r200,"HTTP/1.1 200 O\r\nConnection: keep-alive\r\nContent-Type: text/plain; charset=utf-8\r\nContent-length:     \r\n\r\n");

    r200_headers_len=strlen(r200);
    r200_headers_end_p=&r200[r200_headers_len];
    delete[] r200;
}

#endif // RESP_TEMPLATES_H
