#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP

#include "State.hpp"

#include <array>
using namespace std;

class MCNode;

// Monte Carlo Search Tree class
class MonteCarlo {
public:
    MCNode* root;

    MonteCarlo();
    MonteCarlo(const State& S);
    ~MonteCarlo() { delete root; }

    void select(MCNode*& cur);
    void expand(MCNode*& cur);
    int simulate(MCNode*& cur);
    void backtrack(MCNode*& cur, int winner);

    void iterate(); // complete one iteration
    int bestMove(); // find best move after root
};

// Monte Carlo Search Tree node class
class MCNode {
public:
    State state;
    MCNode* parent;
    int expNum; // number of expanded nodes
    array<MCNode*,25> expanded;
    int totalReward;
    int wonReward;

    MCNode();
    MCNode(const State& S);
    MCNode(const State& S, MCNode* par);
    ~MCNode() {
        delete parent;
        for(auto i : expanded) delete i;
    }
};

#endif