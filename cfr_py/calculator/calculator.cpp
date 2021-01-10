//
// Created by cothrax on 1/5/21.
//

#include "calculator.h"
#include <algorithm>
#include <cstring>
#include <random>
#include <ctime>


void Calculator::gen_cp(Card_power * cp, CType tp, int c1,int c2, int c3, int c4, int c5)
{
    cp->type = tp;
    cp->other[0]=c1; cp->other[1]=c2; cp->other[2]=c3; cp->other[3]=c4; cp->other[4]=c5;
}

#define ISSTRAIGHT(c1,c2,c3,c4,c5) ((c1)==(c2)+1&&(c2)==(c3)+1&&(c3)==(c4)+1&&(((c5)==12&&(c4)==0)||(c4)==(c5)+1))
// c1>c2>c3>c4>c5 [0:12]
int Calculator::rank5_ranged(int c1, int c2, int c3, int c4, int c5, int flush)
{
    Card_power cp;
    if(flush){
        if(ISSTRAIGHT(c1,c2,c3,c4,c5))   
            gen_cp(&cp,SFlush,c1,0,0,0,0);   
        else 
            gen_cp(&cp,Flush,c1,c2,c3,c4,c5);
    }
    else if(ISSTRAIGHT(c1,c2,c3,c4,c5))
        gen_cp(&cp,Straight,c1,0,0,0,0);
    else if(c1==c2&&c2==c3&&c3==c4)
        gen_cp(&cp,FourSame,c1,c5,0,0,0);
    else if(c4==c5&&c2==c3&&c3==c4)
        gen_cp(&cp,FourSame,c2,c1,0,0,0);
    else if(c1==c2&&c2==c3){
        if(c4==c5) gen_cp(&cp,FullHouse,c1,c4,0,0,0);
        else gen_cp(&cp,ThreeSame,c1,c4,c5,0,0);
    }
    else if(c3==c4&&c4==c5){
        if(c1==c2) gen_cp(&cp,FullHouse,c3,c1,0,0,0);
        else gen_cp(&cp,ThreeSame,c3,c1,c2,0,0);
    }
    else if(c2==c3&&c3==c4)
        gen_cp(&cp,ThreeSame,c2,c1,c5,0,0);
    else if(c1==c2){
        if(c3==c4) gen_cp(&cp,TowPairs,c1,c3,c5,0,0);
        else if(c4==c5) gen_cp(&cp,TowPairs,c1,c4,c3,0,0);
        else gen_cp(&cp,Pair,c1,c3,c4,c5,0);
    }
    else if(c2==c3){
        if(c4==c5) gen_cp(&cp,TowPairs,c2,c4,c1,0,0);
        else gen_cp(&cp,Pair,c2,c1,c4,c5,0);
    }
    else if(c3==c4)
        gen_cp(&cp,Pair,c3,c1,c2,c5,0);
    else if(c4==c5)
        gen_cp(&cp,Pair,c4,c1,c2,c3,0);
    else 
        gen_cp(&cp,Highcard,c1,c2,c3,c4,c5);
    return cp.type<<20 | cp.other[0]<<16 | cp.other[1]<<12 | cp.other[2]<<8 | cp.other[3]<<4 | cp.other[4];
}

//static int tb_rank5_flush[371293]; //13^5
//static int tb_rank5_noflush[371293];

#define GET_INDEX5(c1,c2,c3,c4,c5) ((c1)*169*169 + (c2)*169*13 + (c3)*13*13+ (c4)*13 + (c5))
void Calculator::gen_rank5_table(void)
{
    int c[5];
    for(c[0]=0;c[0]<13;c[0]++)
        for(c[1]=0;c[1]<13;c[1]++)
            for(c[2]=0;c[2]<13;c[2]++)
                for(c[3]=0;c[3]<13;c[3]++)
                    for(c[4]=0;c[4]<13;c[4]++){
                        int idx = GET_INDEX5(c[0],c[1],c[2],c[3],c[4]);
                        int d[5];
                        memcpy(d,c,5*sizeof(int));
                        std::sort(d,d+5);
                        tb_rank5_flush[idx] = rank5_ranged(d[4],d[3],d[2],d[1],d[0],1);
                        tb_rank5_noflush[idx] = rank5_ranged(d[4],d[3],d[2],d[1],d[0],0);
                    }
}

void Calculator::init()
{
    // TODO
    tb_rank5_flush = new int[371293];
    tb_rank5_noflush = new int[371293];
    gen_rank5_table();
}

#define ISFLUSH(c1,c2,c3,c4,c5) ((c1)==(c2)&&(c2)==(c3)&&(c3)==(c4)&&(c4)==(c5))
int Calculator::_power(int * holes, int* pubs)
{
    int cards[7] = {holes[0]%13,holes[1]%13,pubs[0]%13,pubs[1]%13,pubs[2]%13,pubs[3]%13,pubs[4]%13};
    int suits[7] = {holes[0]/13,holes[1]/13,pubs[0]/13,pubs[1]/13,pubs[2]/13,pubs[3]/13,pubs[4]/13};
    int i[5];
    int max_power = 0;
    for(i[0]=0;i[0]<3;i[0]++)
        for(i[1]=i[0]+1;i[1]<4;i[1]++)
            for(i[2]=i[1]+1;i[2]<5;i[2]++)
                for(i[3]=i[2]+1;i[3]<6;i[3]++)
                    for(i[4]=i[3]+1;i[4]<7;i[4]++){
                        int idx = GET_INDEX5(cards[i[0]],cards[i[1]],cards[i[2]],cards[i[3]],cards[i[4]]);
                        int flush = ISFLUSH(suits[i[0]],suits[i[1]],suits[i[2]],suits[i[3]],suits[i[4]]);
                        int power = flush>0 ? tb_rank5_flush[idx] : tb_rank5_noflush[idx];
                        if(power>max_power) 
                            max_power = power;
                    }
    return max_power;
}
int Calculator::power(int h0, int h1, int p0, int p1, int p2, int p3, int p4)
{
	int holes[2] = {h0, h1};
	int pubs[5] = {p0, p1, p2, p3, p4};
	return _power(holes,pubs);
}


int Calculator::potential_power(int h0, int h1, int p0, int p1, int p2, int p3, int p4, int step)
{
    // TODO
    if(step == 3) return power(h0, h1, p0, p1, p2, p3, p4);
	int holes[2] = {h0, h1};
	int pubs[5] = {p0, p1, p2, p3, p4};


    int num = step == 0 ? 0 : 2 + step;
    memcpy(cards, holes, sizeof(int) * 2);
    memcpy(cards + 2, pubs, sizeof(int) * num);
    std::sort(cards, cards+num+2);

    ull key = 0;
    for(int i = 0; i < num+2; i++) key = key * 52 + cards[i];
    auto x = pp_cache.find(key);
    if(x != pp_cache.end()) return x->second;

    int iter = MC_ITER / (step + 1);

    memset(mask, 0, sizeof(mask));
    mask[holes[0]] = true;
    mask[holes[1]] = true;
    int cnt = num;

    for(int i = 0; i < num; i++)
    {
        mask[pubs[i]] = true;
        cards[i] = pubs[i];
    }

    for(int i = 0; i < 52; i++) if(!mask[i]) cards[cnt++] = i;

    ll res = 0;
    for(int i = 0; i < iter; i++)
    {
        std::shuffle(cards+num, cards+52, std::mt19937(std::random_device()()));
        res += (ll) power(holes[0], holes[1], cards[0], cards[1], cards[2], cards[3], cards[4]);
    }
    return pp_cache[key] = (int)(res / iter);
}

// int Calculator::opp_potential_power(int *holes, int *pubs, int step)
// {
//     int num = step == 0 ? 0 : 2 + step;
//     memcpy(cards, holes, sizeof(int) * 2);
//     memcpy(cards + 2, pubs, sizeof(int) * num);
//     std::sort(cards, cards+num+2);

//     ull key = 0;
//     for(int i = 0; i < num+2; i++) key = key * 52 + cards[i];
//     auto x = opp_cache.find(key);
//     if(x != opp_cache.end()) return x->second;

//     memset(mask, 0, sizeof(mask));
//     mask[holes[0]] = true;
//     mask[holes[1]] = true;

//     int cnt = num;

//     for(int i = 0; i < num; i++)
//     {
//         mask[pubs[i]] = true;
//         cards[i] = pubs[i];
//     }
//     for(int i = 0; i < 52; i++) if(!mask[i]) cards[cnt++] = i;

//     ll res = 0;
//     for(int i = 0; i < MC_ITER; i++)
//     {
//         std::shuffle(cards+num, cards+52, std::mt19937(std::random_device()()));
//         res += (ll) power(cards+5, cards);
//     }
//     return opp_cache[key] = (int)(res / MC_ITER);
// }

#define USE_CACHE
double Calculator::prior_win_rate(int h0, int h1, int p0, int p1, int p2, int p3, int p4, int step,int NOP,int MCtimes)
{
    int holes[2] = {h0, h1};
	int pubs[5] = {p0, p1, p2, p3, p4};
    if(step>0) step += 2;
#ifdef USE_CACHE
    ull key = step;
    for(int i = 0; i < 2; i++) key = key * 52 + holes[i];
    for(int i = 0; i < step; i++) key = key * 52 + pubs[i];
    auto x = pwr_cache.find(key);
    if(x != pwr_cache.end()) return x->second;
#endif
    //const int NOP = 5;
    int pool[52];
    int sp = 0;
    for(int i=0;i<52;i++){
        int conflict = 0;
        for(int j=0;j<step;j++)
            if(i==pubs[j]){
                conflict = 1;
                break;
            }
        if(i==holes[0]||i==holes[1]||conflict>0)
            continue;
        pool[sp++] = i;
    }

    std::default_random_engine e;
    e.seed(time(NULL));

    int MCpub[5];
    if(step>0)
        memcpy(MCpub,pubs,step*sizeof(int));

    int wins,ties,loses;
    wins = ties = loses = 0;

    for(int i=0;i<MCtimes;i++){
        std::shuffle(pool,pool+sp,e);
        if(step<5)
            memcpy(MCpub+step,pool,(5-step)*sizeof(int));
        int max_oppower = 0;
        for(int j=0;j<NOP;j++){
            int oppower = _power(pool+5-step+2*j,MCpub);
            if(oppower>max_oppower)
                max_oppower = oppower;
        }
        int mypower = _power(holes,MCpub);
        ties += mypower == max_oppower;
        wins += mypower > max_oppower;
        loses += mypower < max_oppower;
    }
    double rate = (wins*1.0 + ties*1.0/2)/MCtimes;
#ifdef USE_CACHE
    pwr_cache[key] = rate;
#endif
    return rate;
}