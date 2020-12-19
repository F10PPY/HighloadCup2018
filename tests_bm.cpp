#include "common.h"


// Utility template function used to print container
template<class T> void PrintContainer(T first, T last)
{
    if (first == last)
        std::cout << "<EMPTY SET>";
    else
        for (; first != last; ++first)
            std::cout << *first << ";";
    std::cout << std::endl;
}
const unsigned max_vectors = 4;
void bm_tests()
{
  //  std::vector<unique_ptr<bm::bvector<>> > bm_inters_vec;
    bm::bvector<> *barr[5];
    bm::aggregator<bm::bvector<> > agg;
    bm::bvector<> bv_res;


   //agg.add()
    const bm::bvector<>* bv=(new bm::bvector<>());
   agg.combine_and(bv_res,barr,4);
   agg.
   agg.combine_and(1,2,bv_res,)

//    for( uint i=0;i<2;i++)
//    {
//        bm_inters_vec[i]->optimize_gap_size();
//        bm_inters_vec[i]->optimize();
//        agg.add(bm_inters_vec[i].get());
//    }
//    agg.set_optimization();
//    ntb::Counter c1;
//    int test=0;
//    for(int n=0;n<1000;n++)
//    {
//        for(int i=max_id;i!=0;i--)
//        {
//                test+=inters_bs_vec[6][i]&inters_bs_vec[7][i]&inters_bs_vec[8][i]&inters_bs_vec[9][i];
//        }
//    }
//    c1.elapsedPrint();
//    cout<<"test: "<<test<<endl;
//    c1.reset();


//    bitset<arr_size>res_bs;


//    uint32_t pos=0;
//    for(int n=0;n<1000;n++)
//    {
//        res_bs=inters_bs_vec[6]&inters_bs_vec[7]&inters_bs_vec[8]&inters_bs_vec[9];
//    }
//    c1.elapsedPrint();
//    c1.reset();
//    for(int n=0;n<1000;n++)
//    {
//        while(pos<max_id)
//        {
//            pos=res_bs._Find_next(pos);
//        }
//        pos=0;
//    }
//    c1.elapsedPrint();
//    cout<<pos<<endl<<endl;

//    c1.reset();
//    for(int n=0;n<1000;n++)
//    {
//        agg.combine_and(bv_res);
//    }
//    c1.elapsedPrint();
//    pos=0;
//    c1.reset();
//    for(int n=0;n<1000;n++)
//    { //bv_res.optimize();
//        while(bv_res.find(pos+1,pos))
//        {
//        }
//        pos=0;
//    }
//    c1.elapsedPrint();
//    cout<<pos<<endl;
//    bm::bvector<>::enumerator en = bv_res.first();
//    bm::bvector<>::enumerator en_end = bv_res.end();

//    c1.reset();
//    for(int n=0;n<1000;n++)
//    {
//        while (en < en_end)
//        {
//            // cout << *en << endl;
//            ++en;  // pre-increment - fastest way to increment enumerator
//        }
//        en = bv_res.first();
//        en_end = bv_res.end();
//    }
//    c1.elapsedPrint();
    //return 0;



//            bm::bvector<>::enumerator en = bv.first();
//            bm::bvector<>::enumerator en_end = bv.end();

//            while (en < en_end)
//            {
//                cout << *en << endl;
//                ++en;  // pre-increment - fastest way to increment enumerator
//            }








}
