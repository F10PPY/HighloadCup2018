#ifndef POST_H
#define POST_H
#include "common.h"
static char r201[]="HTTP/1.1 201 C\r\nConnection: keep-alive\r\nContent-Type: t\r\nContent-length: 0\r\n\r\n";
static char r202[]="HTTP/1.1 202 A\r\nConnection: keep-alive\r\nContent-Type: t\r\nContent-length: 0\r\n\r\n";
static const uint8_t r201_len=static_cast<uint8_t>(strlen(r201));
static const uint8_t r202_len=static_cast<uint8_t>(strlen(r202));
extern ska::flat_hash_map <string,uint8_t> post_mp;
inline void initPostMap()noexcept
{
    post_mp.emplace("id",K_ID);
    post_mp.emplace("email",K_EMAIL);
    post_mp.emplace("phone",K_PHONE);
    post_mp.emplace("interests",K_INTERESTS);
    post_mp.emplace("birth",K_BIRTH);
    post_mp.emplace("sname",K_SNAME);
    post_mp.emplace("sex",K_SEX);
    post_mp.emplace("likes",K_LIKES);
    post_mp.emplace("premium",K_PREMIUM);
    post_mp.emplace("status",K_STATUS);
    post_mp.emplace("fname",K_FNAME);
    post_mp.emplace("joined",K_JOINED);
    post_mp.emplace("country",K_COUNTRY);
    post_mp.emplace("city",K_CITY);
}

#endif // POST_H
