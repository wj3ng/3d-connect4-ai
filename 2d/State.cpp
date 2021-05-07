#include "State.hpp"

#include <iostream> 

#include <array>
#include <cstring>

using namespace std;


State::State()
    : turn(0)
    , linesMade(0)
    , score()
    , mask()
    , stones()
    , legalMoves(7)
    , isIllegal() {
        for(int i=1; i<=7; ++i){
            legalMoves[i-1] = i;
            mask[i] = 1;
        }
    }

State::State(const State& G)
    : turn(G.turn)
    , linesMade(G.linesMade)
    , score(G.score)
    , mask(G.mask)
    , stones(G.stones)
    , legalMoves(G.legalMoves)
    , isIllegal(G.isIllegal) { }

bool State::isLegal(int pos) const {
    return !isIllegal[pos];
}

array<array<int,2>,4> dirs{{
    {{0,1}},
    {{1,0}},
    {{1,1}},
    {{1,-1}}
}};

bool isInbounds(int p, int h) {
    return p > 0
        && p <= 7
        && h > 0
        && h < (1<<7);
}

void State::checkLines(int p, int h) {

    for(int i=0; i<4; ++i) {
        int bef=0, aft=0;
        for(int j=1; j<=3; ++j) {
            int np = p + j*dirs[i][0];
            int nh = (dirs[i][1] > 0) ? h<<(j*dirs[i][1]) : h>>(j*(-dirs[i][1]));
            if(isInbounds(np,nh) && (stones[turn][np] & nh)) ++aft;
            else break;
        }
        for(int j=1; j<=3; ++j) {
            int np = p - j*dirs[i][0];
            int nh = (dirs[i][1] > 0) ? h>>(j*dirs[i][1]) : h<<(j*(-dirs[i][1]));
            if(isInbounds(np,nh) && (stones[turn][np] & nh)) ++bef;
            else break;
        }
        for(int j=3; j<=bef+aft; ++j)
            score[turn] += 100 / ++linesMade;
    }

}

// Pre: pos is legal
void State::makeMove(int pos) {

    int height = mask[pos] + 1;

    mask[pos] |= height;
    stones[turn][pos] |= height;

    if(height == 1<<6) { // if pole is filled
        for(int i=0; i<legalMoves.size(); ++i) {
            if(legalMoves[i] == pos)
                legalMoves.erase(legalMoves.begin() + i);
        }
        isIllegal[pos] = 1;
    }

    checkLines(pos,height);

    turn = !turn;

}

bool State::done() const {
    return legalMoves.empty();
}

int State::winner() const {
    if(score[0] == score[1]) return 2;
    return score[0] > score[1] ? 0 : 1;
}

