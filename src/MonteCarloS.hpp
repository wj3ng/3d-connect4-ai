#ifndef MONTECARLOS_HPP
#define MONTECARLOS_HPP

#include "State.hpp"

#include <array>
using namespace std;

// MCNode with static memory
class MCNodeS {
public: 
    State state;
    int ind, parent;
    int expNum;
    array<int,25> expanded; // 0 if empty
    int totalReward;
    int wonReward;
};

// Monte Carlo Search Tree class using a static array
class MonteCarloS {
public:
    int size;
    array<MCNodeS,500005> tr;

    MonteCarloS();
    MonteCarloS(const State& S);

    void select(int& cur);
    void expand(int& cur);
    int simulate(int& cur);
    void backtrack(int& cur, int winner);

    void iterate(); // complete one iteration
    int bestMove(); // find best move after root
};

#endif