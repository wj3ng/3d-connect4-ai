#include "MonteCarlo.hpp"

#include <iostream>

#include <vector>
#include <chrono>
#include <random>
#include <cmath>

using namespace std;

const double EXP_PARAM = sqrt(2);

unsigned timeSeed = chrono::system_clock::now().time_since_epoch().count();
mt19937 mt(timeSeed);

double ucb1(int wi, int si, int sp) { // wins, total reward, parent total reward 
    // cerr << "ucb1 " << wi << " " << si << " " << sp << endl;
    if(sp == 0. || si == 0.) return 0.;
    double exploit = (double)wi / si;
    double explore = EXP_PARAM * sqrt(log(sp)/si);
    return exploit + explore;
}

// ========== MonteCarlo ========== 

MonteCarlo::MonteCarlo() { }

MonteCarlo::MonteCarlo(const State& S) {
    root = new MCNode(S);
}

// select child with highest UCB1 until node has unexpanded nodes
void MonteCarlo::select(MCNode*& cur) {

    // cerr << cur->expNum << " " << cur->state.legalMoves.size() << endl;
    
    // cerr << "begin: " << cur->state.done();

    while(cur->expNum && cur->expNum == cur->state.legalMoves.size()) {

        double maxUcb1 = 0.;
        MCNode* next = NULL;

        for(MCNode* i : cur->expanded) {
            // cerr << (i?'O':'X');
            if(!i) continue;
            // cerr << i->wonReward << " " << i->totalReward << " " << cur->totalReward << endl;
            double curUcb1 = ucb1(i->wonReward, i->totalReward, cur->totalReward);
            if(curUcb1 > maxUcb1) {
                maxUcb1 = curUcb1;
                next = i;
            }
        }

        // cerr << endl;

        cur = next;

    }

    // cerr << ", end: " << cur->state.done() << endl;

}

// expand one random unexpanded node
void MonteCarlo::expand(MCNode*& cur) {

    // for(auto i:cur->expanded)
    //     cerr << i << endl;

    vector<int> unexpanded;
    for(int i=1; i<=7; ++i) {
        if(cur->state.isLegal(i) && !cur->expanded[i])
            unexpanded.push_back(i);
    }

    if(unexpanded.empty()) return;

    int newMove = unexpanded[mt()%unexpanded.size()];
    // cerr << "new move: " << newMove << endl;

    cur->expanded[newMove] = new MCNode(cur->state, cur);
    ++cur->expNum;

    cur = cur->expanded[newMove];
    cur->state.makeMove(newMove);

}

// simulate game to end from current node, returns winner 
int MonteCarlo::simulate(MCNode*& cur) {

    State curState = cur->state;

    while(!curState.done()){
        int newMove = curState.legalMoves[mt()%curState.legalMoves.size()];
        curState.makeMove(newMove);
    }

    return curState.winner();

}

// update simulation results from current node to root
void MonteCarlo::backtrack(MCNode*& cur, int winner) {

    while(cur) {

        cur->totalReward += 2;
        if(winner == 2) cur->wonReward += 1;
        else if(winner != cur->state.turn) cur->wonReward += 2;

        cur = cur->parent;

    }

}

void MonteCarlo::iterate() {
    MCNode* cur = root;
    // cerr << "selecting\n";
    select(cur);
    // cerr << "expanding\n";
    expand(cur);
    // cerr << "simulating\n";
    int winner = simulate(cur);
    // cerr << "backtracking\n";
    backtrack(cur, winner);
}

int MonteCarlo::bestMove() {
    int maxReward = -1, ret;
    for(int i=1; i<=7; ++i) {
        if(root->expanded[i] && root->expanded[i]->totalReward > maxReward) {
            ret = i;
            maxReward = root->expanded[i]->totalReward;
        }
    }
    return ret;
}

// ========== MCNode ========== 

MCNode::MCNode()
    : expanded() { }

MCNode::MCNode(const State& S)
    : state(S)
    , parent(NULL)
    , expNum(0)
    , expanded()
    , totalReward(0)
    , wonReward(0) { }

MCNode::MCNode(const State& S, MCNode* par)
    : state(S)
    , parent(par)
    , expNum(0)
    , expanded()
    , totalReward(0)
    , wonReward(0) { }