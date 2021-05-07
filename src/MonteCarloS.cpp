#include "MonteCarloS.hpp"

#include <vector>
#include <chrono>
#include <random>
#include <cmath>

using namespace std;

const double EXP_PARAM = sqrt(2); // exploration parameter for ucb1

// initialize + seed randomizer
unsigned timeSeed = chrono::system_clock::now().time_since_epoch().count();
mt19937 mt(timeSeed);

// upper confidence bound 1 formula
double ucb1(int wi, int si, int sp) { // wins, total reward, parent total reward 
    if(si == 0.) return 0.;
    double exploit = (double)wi / si;
    double explore = EXP_PARAM * sqrt(log(sp)/si);
    return exploit + explore;
}

// ========== MonteCarloS ========== 

MonteCarloS::MonteCarloS()
    : size(1) { }

MonteCarloS::MonteCarloS(const State& S)
    : size(1) {
        tr[0].state = S;
    }

// select child with highest UCB1 until node has unexpanded nodes
void MonteCarloS::select(int& cur) {

    while(!tr[cur].state.done() && tr[cur].expNum == tr[cur].state.legalMoves.size()) {

        double maxUcb1 = 0.;
        int next = 0;

        for(int i : tr[cur].expanded) {
            if(!i) continue;
            double curUcb1 = ucb1(tr[i].wonReward, tr[i].totalReward, tr[i].totalReward);
            if(curUcb1 > maxUcb1) {
                maxUcb1 = curUcb1;
                next = i;
            }
        }

        cur = next;

    }

}

// expand one random unexpanded node
void MonteCarloS::expand(int& cur) {

    vector<int> unexpanded;
    for(int i=1; i<=24; ++i) {
        if(tr[cur].state.isLegal(i) && !tr[cur].expanded[i])
            unexpanded.push_back(i);
    }

    if(tr[cur].state.done()) return;

    int newMove = unexpanded[mt()%unexpanded.size()];

    tr[cur].expanded[newMove] = size;
    tr[size].parent = cur;
    tr[size].state = tr[cur].state;
    ++size;
    
    ++tr[cur].expNum;

    cur = tr[cur].expanded[newMove];
    tr[cur].state.makeMove(newMove);

}

// simulate game to end from current node, returns winner 
int MonteCarloS::simulate(int& cur) {

    State curState = tr[cur].state;

    while(!curState.done()){
        int newMove = curState.legalMoves[mt()%curState.legalMoves.size()];
        curState.makeMove(newMove);
    }

    return curState.winner();

}

// update simulation results from current node to root
void MonteCarloS::backtrack(int& cur, int winner) {

    while(cur) {

        tr[cur].totalReward += 2;
        if(winner == 2) tr[cur].wonReward += 1;
        else if(winner != tr[cur].state.turn) tr[cur].wonReward += 2;

        cur = tr[cur].parent;

    }

    // update root
    tr[0].totalReward += 2;
    if(winner == 2) tr[0].wonReward += 1;
    else if(winner != tr[0].state.turn) tr[0].wonReward += 2;

}

void MonteCarloS::iterate() {
    int cur = 0;
    select(cur);
    expand(cur);
    int winner = simulate(cur);
    backtrack(cur, winner);
}

int MonteCarloS::bestMove() {
    int maxReward = -1, ret;
    for(int i=1; i<=24; ++i) {
        if(tr[0].expanded[i] && tr[tr[0].expanded[i]].totalReward > maxReward) {
            ret = i;
            maxReward = tr[tr[0].expanded[i]].totalReward;
        }
    }
    return ret;
}