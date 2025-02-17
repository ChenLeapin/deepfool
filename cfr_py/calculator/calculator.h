//
// Created by cothrax on 1/5/21.
//

#ifndef DEEPFOOL_CALCULATOR_H
#define DEEPFOOL_CALCULATOR_H

#include <unordered_map>
using std::unordered_map;
typedef long long ll;
typedef unsigned long long ull;
#define POWER_RANGE  1000 // TODO
#define MAX_POWER    9175040
#define MC_ITER      200

// 花色
// 牌的编码为 (花色*13 + 数字)，范围[0, 51]
// A 的编码为12而不是0 (可以改)
#define SPADE 0
#define DIAMOND 1
#define CLUB 2
#define HEART 3

typedef enum {Highcard=0,Pair,TowPairs,ThreeSame,Straight,Flush,FullHouse,FourSame,SFlush} CType;
typedef struct {
    CType type;
    char other[5];
} Card_power;

class Calculator {
    int *tb_rank5_flush;
    int *tb_rank5_noflush;

    bool mask[52];
    int cards[52];

private:
    // 你也许需要初始化一些东西？
    void init();
    unordered_map<ull, int> pp_cache;
    unordered_map<ull, double> pwr_cache;
    int _power(int * holes, int* pubs);
    // unordered_map<ull, int> opp_cache;

public:
    Calculator() { init(); }

    void gen_rank5_table(void);
    int rank5_ranged(int c1, int c2, int c3, int c4, int c5, int flush);
    void gen_cp(Card_power * cp, CType tp, int c1,int c2, int c3, int c4, int c5);

    // 牌力
    // 底牌 holes, 5张公共牌 pubs
    // done
    // int power(int *holes, int *pubs);
	int power(int h0, int h1, int p0, int p1, int p2, int p3, int p4);

    // 玩家的先验期望牌力
    // 阶段 step (game.h)
    int potential_power(int h0, int h1, int p0, int p1, int p2, int p3, int p4, int step);

    // 任一对手的先验期望牌力
    // 阶段 step (game.h)
    // int opp_potential_power(int *holes, int *pubs, int step);

    // 当前step下给定底牌的胜率 = [0,1]（其它玩家底牌未知，不考虑下注动作）
    // step = {0,1,2,3} 
    // NOP: 本场入局的对手数 = {1,2,3,4,5}
    double prior_win_rate(int h0, int h1, int p0, int p1, int p2, int p3, int p4, int step,int NOP=5,int MCtimes=20000);

};

#endif //DEEPFOOL_CALCULATOR_H
