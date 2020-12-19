#include "util.h"
#include "chunked_bm_vec.h"

//#ifdef TEST
uint32_t max_id=30000;
uint32_t max_id_reverse=0;
//#else
//uint32_t max_id=1300000;
//#endif

ska::flat_hash_map <string,uint8_t>dic_domain;
ska::flat_hash_map <string,uint8_t>dic_country;
ska::flat_hash_map <string,uint32_t>dic_city;
ska::flat_hash_map <string,uint8_t>dic_interest;
ska::flat_hash_map <string,uint8_t>dic_fname;
tsl::htrie_map<char,uint16_t>dic_sname;
uint8_t dic_domain_val=0;
uint8_t dic_country_val=1;
uint32_t dic_city_val=1;
uint8_t dic_interest_val=0;
uint8_t dic_fname_val=1;
uint16_t dic_sname_val=1;
vector<StrP256Max> dic_interest_strings;
vector<StrP256Max> dic_country_strings;
vector<StrP256Max> dic_city_strings;
vector<StrP256Max> dic_fname_strings;
vector<StrP256Max> dic_sname_strings;
vector<bitset<arr_size>>inters_bs_vec;
array<Account,arr_size+1>accounts;

uint32_t options_ts;
bool is_rating=false;
std::vector<unique_ptr<bm::bvector<>> > bm_inters_vec;
std::vector<unique_ptr<bm::bvector<>> > reverse_inters_vec;




vector<ChunkedBmVec> chunked_inters_vec;
vector<ChunkedBmVec> chunked_country_vec;
vector<ChunkedBmVec> chunked_city_vec;
vector<ChunkedBmVec> chunked_fname_vec;
//vector<ChunkedBmVec> chunked_sname_vec;
vector<ChunkedBmVec> chunked_sex_vec;
vector<ChunkedBmVec> chunked_birth_year_vec;
//vector<ChunkedBmVec> chunked_phone_code_vec;

void loadData()
{
    dic_country_strings.emplace_back("");
    dic_city_strings.emplace_back("");
    dic_sname_strings.emplace_back("");
    dic_fname_strings.emplace_back("");
    chunked_country_vec.emplace_back((arr_size));
    chunked_city_vec.emplace_back((arr_size));
    chunked_fname_vec.emplace_back((arr_size));
   // chunked_sname_vec.emplace_back((arr_size));
    chunked_sex_vec.emplace_back((arr_size));//m
    chunked_sex_vec.emplace_back((arr_size));//f
    for(size_t i=0;i<56;i++)
    {
        chunked_birth_year_vec.emplace_back((arr_size));
    }


    st_counter.reset();
    time_t timeBorn;
    tm* tm_struct;
#ifndef LOCAL
    // if(fs::exists("../tmp/data/data.zip"))
    //system("unzip -o ../tmp/data/data.zip -d unz >/dev/null");
    //  else  system("unzip -o data.zip -d unz >/dev/null");
    system("unzip -o ../tmp/data/data.zip -d unz >/dev/null");
    //  ntb::clElapsedPrint("unzipped");
#endif

#ifndef LOCAL
    ifstream o("../tmp/data/options.txt");
#else
    ifstream o(DATA_PATH"/opt/options.txt");
#endif


    o>>options_ts;
#ifdef DEBUG
    o>>is_rating;
#endif
    o.close();
    const char* pos;
    ptrdiff_t pdiff;
    string status;
    Account* ref;
    uint32_t id;
    Value::ConstMemberIterator itr;

    for (auto & p : fs::directory_iterator(DATA_PATH))
    {
        if(fs::is_directory(p))continue;
        ifstream i(p.path());
        if(!i.is_open()){cout<<"can't open"<<p.path()<<endl;return;}
        IStreamWrapper isw(i);
        Document doc;
        doc.ParseStream(isw);

        for (Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
        {
            for (SizeType i = 0; i < it->value.Size(); i++)
            {
                id=it->value[i]["id"].GetUint();
                ref=&accounts[id];
                ref->birth_ts=it->value[i]["birth"].GetUint();
                timeBorn=ref->birth_ts;

                tm_struct=gmtime(&timeBorn);
                ref->birth_year=tm_struct->tm_year-50;                  //BIRTH
                chunked_birth_year_vec[ref->birth_year].SetBit(arr_size-id);

                ref->joined=it->value[i]["joined"].GetUint();                  //JOINED

                if(!strcmp("заняты",it->value[i]["status"].GetString()))ref->status=ENGAGED;                  //STATUS
                else if (!strcmp("свободны",it->value[i]["status"].GetString()))ref->status=SINGLE;
                else ref->status=COMPLICATED;

                if('m'==it->value[i]["sex"].GetString()[0])                //SEX
                {
                    ref->sex=MALE;
                    chunked_sex_vec[MALE].SetBit(arr_size-id);
                }
                else
                {
                    ref->sex=FEMALE;
                    chunked_sex_vec[FEMALE].SetBit(arr_size-id);
                }

                itr=it->value[i].FindMember("email");                         //EMAIL
                ref->email.len=itr->value.GetStringLength();
                ref->email.str_p=new char[ref->email.len];
                memcpy(ref->email.str_p, itr->value.GetString(), ref->email.len);

                pos=strchr(ref->email.str_p,'@');
                pdiff=&ref->email.str_p[ref->email.len]-pos;

                string dummy(pos+1,pdiff-1);                  //DOMAIN
                auto um_it=dic_domain.find(dummy);
                if (dic_domain.end()==um_it)
                {
                    dic_domain.emplace(dummy,dic_domain_val);
                    ref->email_domain=dic_domain_val++;
                }
                else ref->email_domain=um_it->second;

                itr=it->value[i].FindMember("country");                  //COUNTRY
                if ( itr!= it->value[i].MemberEnd())
                {
                    dummy=itr->value.GetString();
                    auto &&um_it=dic_country.find(dummy);
                    if (dic_country.end()==um_it)
                    {
                        dic_country_strings.emplace_back(dummy.data());
                        dic_country.emplace(dummy,dic_country_val);
                        ref->country=dic_country_val;

                        chunked_country_vec.emplace_back((arr_size));
                        chunked_country_vec[dic_country_val].SetBit(arr_size-id);

                        dic_country_val++;
                    }
                    else
                    {
                        ref->country=um_it->second;
                        chunked_country_vec[um_it->second].SetBit(arr_size-id);
                    }
                }
                else  chunked_country_vec[EMPTY].SetBit(arr_size-id);



                itr=it->value[i].FindMember("city");                  //CITY
                if ( itr!= it->value[i].MemberEnd())
                {
                    dummy=itr->value.GetString();
                    auto &&um_it=dic_city.find(dummy);
                    if (dic_city.end()==um_it)
                    {
                        dic_city_strings.emplace_back(dummy.data());
                        dic_city.emplace(dummy,dic_city_val);
                        ref->city=dic_city_val;

                        chunked_city_vec.emplace_back((arr_size));
                        chunked_city_vec[dic_city_val].SetBit(arr_size-id);

                        dic_city_val++;
                    }
                    else
                    {
                        ref->city=um_it->second;
                        chunked_city_vec[um_it->second].SetBit(arr_size-id);
                    }
                }
                else  chunked_city_vec[EMPTY].SetBit(arr_size-id);

                itr=it->value[i].FindMember("fname");                  //FNAME
                if ( itr!= it->value[i].MemberEnd())
                {
                    dummy=itr->value.GetString();
                    auto &&um_it=dic_fname.find(dummy);
                    if (dic_fname.end()==um_it)
                    {
                        dic_fname_strings.emplace_back(dummy.data());
                        dic_fname.emplace(dummy,dic_fname_val);
                        ref->fname=dic_fname_val;

                        chunked_fname_vec.emplace_back((arr_size));
                        chunked_fname_vec[dic_fname_val].SetBit(arr_size-id);
                        dic_fname_val++;
                    }
                    else
                    {
                        ref->fname=um_it->second;
                        chunked_fname_vec[um_it->second].SetBit(arr_size-id);
                    }
                }
                else  chunked_fname_vec[EMPTY].SetBit(arr_size-id);

                itr=it->value[i].FindMember("sname");                  //SNAME
                if ( itr!= it->value[i].MemberEnd())
                {
                    //                    ref->sname.len=itr->value.GetStringLength();
                    //                    ref->sname.str_p=new char[ref->sname.len];
                    //                    memcpy(ref->sname.str_p, itr->value.GetString(), ref->sname.len);
                    dummy=itr->value.GetString();
                    auto &&um_it=dic_sname.find(dummy);
                    if (dic_sname.end()==um_it)
                    {
                        dic_sname_strings.emplace_back(dummy.data());
                        dic_sname.emplace(dummy,dic_sname_val);
                        ref->sname=dic_sname_val;

                      //  chunked_sname_vec.emplace_back((arr_size));
                      //  chunked_sname_vec[dic_sname_val].SetBit(arr_size-id);
                        dic_sname_val++;
                    }
                    // else ref->sname=um_it->second;
                    else
                    {
                        ref->sname=um_it.value();
                       // chunked_sname_vec[um_it.value()].SetBit(arr_size-id);
                    }

                }
                // else  chunked_sname_vec[EMPTY].SetBit(arr_size-id);

                itr=it->value[i].FindMember("phone");                  //PHONE
                if ( itr!= it->value[i].MemberEnd())
                {
                    ref->phone.len=itr->value.GetStringLength();
                    ref->phone.str_p=new char[ref->phone.len];
                    memcpy(ref->phone.str_p, itr->value.GetString(), ref->phone.len);
                    ref->phone_code=charrGetUint16(&ref->phone.str_p[2]);

                  // chunked_phone_code_vec.emplace_back((arr_size)).SetBit(arr_size-id);
                }

                itr=it->value[i].FindMember("interests");                  //INTERESRS
                if ( itr!= it->value[i].MemberEnd())
                {
                    for (SizeType j = 0; j <  itr->value.GetArray().Size(); j++)
                    {
                        dummy=itr->value[j].GetString();
                        auto &&um_it=dic_interest.find(dummy);
                        if (dic_interest.end()==um_it)
                        {
                            dic_interest_strings.emplace_back(dummy.data());
                            dic_interest.emplace(dummy,dic_interest_val);

                            ref->interests_bs[dic_interest_val]=true;
                            inters_bs_vec.emplace_back()[id]=true;


                            std::unique_ptr<bm::bvector<>> bv(new bm::bvector<>());
                            bm_inters_vec.emplace_back(std::move(bv));
                            bm_inters_vec[dic_interest_val]->set(id);

                            std::unique_ptr<bm::bvector<>> bvr(new bm::bvector<>());
                            reverse_inters_vec.push_back(std::move(bvr));
                            reverse_inters_vec[dic_interest_val]->set(arr_size-id);


                            chunked_inters_vec.emplace_back((arr_size));
                            chunked_inters_vec[dic_interest_val].SetBit(arr_size-id);

                            dic_interest_val++;
                        }
                        else
                        {
                            inters_bs_vec[um_it->second][id]=true;
                            ref->interests_bs[um_it->second]=true;
                            bm_inters_vec[um_it->second]->set(id);
                            reverse_inters_vec[um_it->second]->set(arr_size-id);


                            chunked_inters_vec[um_it->second].SetBit(arr_size-id);
                        }

                    }
                }

                itr=it->value[i].FindMember("likes");                  //LIKES
                if ( itr!= it->value[i].MemberEnd())
                {
                    Like lk;
                    for (SizeType j = 0; j <  itr->value.GetArray().Size(); j++)
                    {
                        lk.likee_id=itr->value[j].GetObject()["id"].GetUint();
                        lk.ts=itr->value[j].GetObject()["ts"].GetUint();
                        ref->likes.emplace_back(lk);

                        accounts[lk.likee_id].likers[ref->sex].emplace_back().liker_id=id;
                        accounts[lk.likee_id].likers[ref->sex].back().ts=lk.ts;

                    }
                    //                    for (SizeType j = 0; j <  itr->value.GetArray().Size(); j++)
                    //                    {
                    //                        ref->likes .emplace(itr->value[j].GetObject()["id"].GetInt(),itr->value[j].GetObject()["ts"].GetInt());
                    //                    }
                    sort(ref->likes.rbegin(),ref->likes.rend());
                }

                itr=it->value[i].FindMember("premium");                  //PREMIUM
                if ( itr!= it->value[i].MemberEnd())
                {
                    if(itr->value.HasMember("start"))
                    {
                        ref->prem_start=itr->value["start"].GetUint();
                    }

                    if(itr->value.HasMember("finish"))
                    {
                        ref->prem_finish=itr->value["finish"].GetUint();
                        if(ref->prem_finish>options_ts)ref->prem_now=true;
                    }
                }

                if(max_id<it->value[i]["id"].GetUint())max_id=it->value[i]["id"].GetUint();

            }
        }
        i.close();
    }

    st_counter.elapsedPrint(DATA_PATH);
    max_id_reverse=arr_size-max_id;

//    for(auto &&it:chunked_inters_vec)
//    {
//        for(auto &&i:it.chunk_arr)
//        {
//            i.get()->optimize();
//        }
//    }

    //#ifdef DEBUG
    //    cout<<dic_interest_strings.size()<<endl;
    //    cout<<dic_country_strings.size()<<endl;
    //    cout<<dic_city_strings.size()<<endl;
    //    cout<<dic_fname_strings.size()<<endl;
    //    cout<<dic_sname_strings.size()<<endl;
    //#endif

}
