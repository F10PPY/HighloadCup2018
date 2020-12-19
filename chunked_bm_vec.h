#ifndef CHUNKED_BM_VEC_H
#define CHUNKED_BM_VEC_H
#include "common.h"

struct ChunkedBmVec
{
    static constexpr  uint8_t num_chunks=4;
    static uint32_t chunk_sz;//1331200/4=332800/2=166400 or 32768/4=8192
    array <unique_ptr<bm::bvector<>>,num_chunks>chunk_arr;
    ChunkedBmVec(uint32_t total)
    {
        chunk_sz=total/num_chunks;
        for(int i=0;i<num_chunks;i++)
        {
            std::unique_ptr<bm::bvector<>> bvr(new bm::bvector<>());
            chunk_arr[i]=std::move(bvr);
        }
    }
    void SetBit(uint32_t i)
    {
        chunk_arr[i/chunk_sz]->set_bit(i%chunk_sz);
    }
    auto getChunk(uint8_t chunk_n)
    {
        return chunk_arr[chunk_n].get();
    }
};
enum ops{COMB_AND,COMB_OR,COMB_ANDSUB,AGGR_AND,AGGR_OR,SET_AGGREGATE};
struct step
{
    //    step(step&&from)
    //    {
    //     operation=from.operation;
    //     step_agg=move(from.step_agg);
    //    }
    step(uint8_t op):operation(op)
    {
        step_agg=new(bm::aggregator<bm::bvector<>>);
    }
    step()
    {
        delete step_agg;
    }
    uint8_t operation;
    bm::aggregator<bm::bvector<> > *step_agg;
    vector <ChunkedBmVec*>step_src_vec;
    bm::bvector<>step_res;
    void addAggSrc(ChunkedBmVec *src)
    {
        step_src_vec.push_back(src);
    }
    bm::bvector<>* run(uint8_t current_chunk)
    {
        switch (operation)
        {
        case AGGR_OR:
            for(auto&&it:step_src_vec)
            {
                step_agg->add(it->getChunk(current_chunk));
            }
            step_agg->combine_or(step_res);
            break;
        }
        return &step_res;
    }
};
struct ChunkedAggregator
{
    uint32_t chunk_size;
    uint8_t current_chunk=0;
    bm::aggregator<bm::bvector<> > final_and_agg;
    bm::bvector<>result;
    bm::bvector<>::enumerator en=result.end();
    vector <const bm::bvector<>*>final_and_group;
    vector <ChunkedBmVec*> and_src_vec;
    vector<step> steps;

    void reset()
    {
        steps.clear();
        final_and_group.clear();
        and_src_vec.clear();
        current_chunk=0;

    }
    step& addAggStep(uint8_t op)
    {
        steps.emplace_back(op);
        return steps.back();
    }
    void add(ChunkedBmVec &src)
    {
        and_src_vec.push_back(&src);
    }
    void findFirst()
    {
         for(auto &&it:and_src_vec)     //adding src
        {
            final_and_group.push_back(it->getChunk(current_chunk));
        }
        for(auto &&step_it:steps)
        {
            final_and_group.push_back(step_it.run(current_chunk));
        }
        if(final_and_group.size()>2)
        {
            final_and_agg.reset();
            for(auto&&it:final_and_group)
            {
                final_and_agg.add(it);
            }
            final_and_agg.combine_and(result);
            en=result.first();
        }
        else if(final_and_group.size()==2)
        {
            result=*final_and_group[0]&*final_and_group[1];
            en=result.first();
        }
        else en=final_and_group[0]->first(); //nothing to compare

        //        if(en<final_res_vec[0]->getChunk(current_chunk)->end())
        //        {
        //            pos=arr_size-(final_res_vec[0]->chunk_sz*current_chunk+*en);//get offset
        //            ++en;
        //            return true;
        //        }
        //        return false;
    }
    bool getNextBit(uint32_t &pos)
    {
        if(en<result.end())
        {
            pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en);
            ++en;
            return true;
        }

        while(++current_chunk<ChunkedBmVec::num_chunks)
        {
            final_and_group.clear();

            for(auto &&it:and_src_vec)     //adding src
            {
                final_and_group.push_back(it->getChunk(current_chunk));
            }
            for(auto &&step_it:steps)
            {
                step_it.step_agg->reset();
                final_and_group.push_back(step_it.run(current_chunk));
            }

            if(final_and_group.size()>2)
            {
                final_and_agg.reset();
                for(auto&&it:final_and_group)
                {
                    final_and_agg.add(it);
                }
                final_and_agg.combine_and(result);
                en=result.first();
            }
            else if(final_and_group.size()==2)
            {
                result=*final_and_group[0]&*final_and_group[1];
                en=result.first();
            }
            else en=final_and_group[0]->first(); //nothing to compare


            if(en<result.end())
            {
                pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en);
                ++en;
                return true;
            }
        }
        return false;
    }
};





struct ChunkedVector
{
    uint32_t chunk_size;
    uint8_t current_chunk=0;
    bm::aggregator<bm::bvector<> > final_agg;
    bm::bvector<>final_res;
    bm::bvector<>::enumerator en=final_res.end();
    //  vector <ChunkedBmVec*>final_res_vec;
    vector <bm::bvector<>*>final_res_vec;
    vector <ChunkedBmVec*> src_vec;
    vector<step> steps;

    step& addAggStep(uint8_t op)
    {
        steps.emplace_back(op);
        return steps.back();
    }
    void add(ChunkedBmVec &src)
    {
        src_vec.push_back(&src);
    }

    bool init()
    {
        for(auto &&it:src_vec)     //copying src
        {
            final_res_vec.push_back(it->getChunk(current_chunk));
        }
        for(auto &&step_it:steps)
        {
            switch (step_it.operation)
            {
            case AGGR_AND:
                for(auto&&it:step_it.step_src_vec)
                {
                    step_it.step_agg->add(it->getChunk(current_chunk));
                }
                step_it.step_agg->combine_and(step_it.step_res);
                final_res_vec.push_back(&step_it.step_res);
                break;
            case AGGR_OR:
                for(auto&&it:step_it.step_src_vec)
                {
                    step_it.step_agg->add(it->getChunk(current_chunk));
                }
                step_it.step_agg->combine_or(step_it.step_res);
                final_res_vec.push_back(&step_it.step_res);

                break;
            }
        }
        if(final_res_vec.size()>2)
        {
            for(auto&&it:final_res_vec)
            {
                final_agg.add(it);
            }
            final_agg.combine_and(final_res);
            en=final_res.first();
            //            if(en<final_res.end()) //probably need this
            //            {
            //                pos=arr_size-(final_src_vec[0]->chunk_sz*current_chunk+*en);//get offset
            //                ++en;
            //                return true;
            //            }
        }
        else if(final_res_vec.size()==2)
        {
            // final_res.bit_and(&final_res_vec[0],&final_res_vec[1],1);
            final_res_vec[0]->combine_operation_and(*final_res_vec[1]);
            en=final_res_vec[0]->first();
        }
        else if(final_res_vec.size()==1) en=final_res_vec[0]->first(); //nothing to compare
        else return false;
        return true;
        //        if(en<final_res_vec[0]->getChunk(current_chunk)->end())
        //        {
        //            pos=arr_size-(final_res_vec[0]->chunk_sz*current_chunk+*en);//get offset
        //            ++en;
        //            return true;
        //        }
        //        return false;
    }
    bool getNextBit(uint32_t &pos)
    {
        if(en<final_res_vec[0]->end())
        {
            pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en);
            ++en;
            return true;
        }


        while(++current_chunk<ChunkedBmVec::num_chunks)
        {
            final_res_vec.clear();

            for(auto &&it:src_vec)     //copying src
            {
                final_res_vec.push_back(it->getChunk(current_chunk));
            }
            for(auto &&stit:steps)
            {
                switch (stit.operation)
                {
                case AGGR_AND:
                    stit.step_agg->reset();
                    for(auto&&it:stit.step_src_vec)
                    {
                        stit.step_agg->add(it->getChunk(current_chunk));
                    }
                    stit.step_agg->combine_and(stit.step_res);
                    final_res_vec.push_back(&stit.step_res);
                    break;
                case AGGR_OR:
                    stit.step_agg->reset();
                    for(auto&&it:stit.step_src_vec)
                    {
                        stit.step_agg->add(it->getChunk(current_chunk));
                    }
                    stit.step_agg->combine_or(stit.step_res);
                    final_res_vec.push_back(&stit.step_res);

                    break;
                }
            }
            if(final_res_vec.size()>2)
            {
                final_agg.reset();
                for(auto&&it:final_res_vec)
                {
                    final_agg.add(it);
                }
                final_agg.combine_and(final_res);
                en=final_res.first();
            }
            else if(final_res_vec.size()==2)
            {
                final_res_vec[0]->combine_operation_and((*final_res_vec[1]));
                en=final_res_vec[0]->first();
            }
            else  en=final_res_vec[0]->first(); //nothing to compare

            if(en<final_res_vec[0]->end())
            {
                pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en); //THIS IS CORRECT
                // pos=arr_size-final_res_vec.size()+*en;
                ++en;
                return true;
            }
        }
        return false;
    }
    //    bool getNextOnBit(uint32_t &pos)
    //    {

    //        if(en<final_res.end())
    //        {
    //            pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en);
    //            ++en;
    //            return true;
    //        }

    //        while(++current_chunk<ChunkedBmVec::num_chunks)//4
    //        {
    //            final_agg.reset();
    //            for(auto&&it:final_res_vec)
    //            {
    //                final_agg.add(it->getChunk(current_chunk));
    //            }
    //            // agg.set_optimization();
    //            final_agg.combine_and(final_res);
    //            final_agg.reset();
    //            en=final_res.first();
    //            if(en<final_res.end())
    //            {
    //                pos=arr_size-(ChunkedBmVec::chunk_sz*current_chunk+*en);
    //                ++en;
    //                return true;
    //            }


    //        }
    //        return false;
    //    }

};
extern vector<ChunkedBmVec> chunked_inters_vec;
extern vector<ChunkedBmVec> chunked_country_vec;
extern vector<ChunkedBmVec> chunked_city_vec;
extern vector<ChunkedBmVec> chunked_fname_vec;
//extern vector<ChunkedBmVec> chunked_sname_vec;
extern vector<ChunkedBmVec> chunked_sex_vec;
extern vector<ChunkedBmVec> chunked_birth_year_vec;
//extern vector<ChunkedBmVec> chunked_phone_code_vec;
#endif // CHUNKED_BM_VEC_H
