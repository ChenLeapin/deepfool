//
// Created by cothrax on 1/5/21.
//

#ifndef DEEPFOOL_CALCULATOR_H
#define DEEPFOOL_CALCULATOR_H

typedef long long ll;
#define POWER_RANGE 1000 // TODO

// 花色
// 牌的编码为 (花色*13 + 数字)，范围[0, 51]
// A 的编码为12而不是0 (可以改)
#define SPADE 0
#define DIAMOND 1
#define CLUB 2
#define HEART 3

class Calculator {
private:
    // 你也许需要初始化一些东西？
    void init();
public:
    Calculator() { init(); }

    // 牌力
    // 底牌 holes, 5张公共牌 pubs
    // done
    int power(int *holes, int *pubs);

    // 玩家的先验期望牌力
    // 底牌 holes, step张公共牌 pubs
    // undone
    int potential_power(int *holes, int *pubs, int step);

    // 任一对手的先验期望牌力
    // 底牌 holes, step张公共牌 pubs
    // undone
    int opp_potential_power(int *holes, int *pubs, int step);
};

extern Calculator calculator;

#endif //DEEPFOOL_CALCULATOR_H
