#include "State.hpp"

#include <array>
#include <cstring>

using namespace std;

array<pair<int,int>,42> index2D {{ // 1-24, out of bounds: 30-41
    {0,0}, // 0
    {0,2}, 
    {0,3},
    {1,1},
    {1,2},
    {1,3}, // 5
    {1,4},
    {2,0},
    {2,1},
    {2,2},
    {2,3}, // 10 
    {2,4},
    {2,5},
    {3,0},
    {3,1},
    {3,2}, // 15
    {3,3},
    {3,4},
    {3,5},
    {4,1},
    {4,2}, // 20
    {4,3},
    {4,4},
    {5,2},
    {5,3},
    {0,0}, // 25
    {0,0}, 
    {0,0}, 
    {0,0}, 
    {0,0}, 
    {0,0}, // 30
    {0,1}, 
    {0,4},
    {0,5},
    {1,0},
    {1,5}, // 35
    {4,0},
    {4,5},
    {5,0},
    {5,1},
    {5,4}, // 40
    {5,5}
}};

const array<array<int,3>,13> dirs {{
    {{0, 0, 1}},
    {{0, 1, 0}},
    {{1, 0, 0}},
    {{0, 1, 1}},
    {{0, 1, -1}},
    {{1, 0, 1}},
    {{1, 0, -1}},
    {{1, 1, 0}},
    {{1, -1, 0}},
    {{1, 1, 1}},
    {{1, 1, -1}},
    {{1, -1, 1}},
    {{1, -1, -1}}
}};

// check if coordinate is within stones
bool isInbounds(int r, int c, int h) {
    return r >= 0
        && r <= 5
        && c >= 0
        && c <= 5
        && h > 0
        && h < (1<<7);
}

State::State()
    : turns(0)
    , turn(0)
    , linesMade(0)
    , score()
    , mask()
    , stones()
    , legalMoves(24)
    , isIllegal() {
        for(int i=1; i<=24; ++i) {
            legalMoves[i-1] = i;
            mask[index2D[i].first][index2D[i].second] = 1;
        }
    }

State::State(const State& G)
    : turns(G.turns)
    , turn(G.turn)
    , linesMade(G.linesMade)
    , score(G.score)
    , mask(G.mask)
    , stones(G.stones)
    , legalMoves(G.legalMoves)
    , isIllegal(G.isIllegal) { }

bool State::isLegal(int pos) const {
    return !isIllegal[index2D[pos].first][index2D[pos].second];
}

void State::checkLines(int r, int c, int h) {
    for(int i=0; i<13; ++i) {

        int bef=0, aft=0;
        for(int j=1; j<=3; ++j) {
            int nr = r + j*dirs[i][0];
            int nc = c + j*dirs[i][1];
            int nh = (dirs[i][2] > 0) ? h<<(j*dirs[i][2]) : h>>(j*(-dirs[i][2]));
            if(isInbounds(nr, nc, nh) && (stones[turn][nr][nc] & nh))
                ++aft;
            else
                break;
        }
        for(int j=1; j<=3; ++j) {
            int nr = r - j*dirs[i][0];
            int nc = c - j*dirs[i][1];
            int nh = (dirs[i][2] > 0) ? h>>(j*dirs[i][2]) : h<<(j*(-dirs[i][2]));
            if(isInbounds(nr, nc, nh) && (stones[turn][nr][nc] & nh))
                ++bef;
            else
                break;
        }

        for(int j=3; j<=bef+aft; ++j)
            score[turn] += 100 / ++linesMade;

    }
}

// Pre: pos is legal
void State::makeMove(int pos) {

    int r = index2D[pos].first, c = index2D[pos].second;
    int height = mask[r][c] + 1;

    mask[r][c] |= height;
    stones[turn][r][c] |= height;

    if(height == 1<<6) { // if pole is filled
        for(int i=0; i<legalMoves.size(); ++i) {
            if(legalMoves[i] == pos)
                legalMoves.erase(legalMoves.begin() + i);
        }
        isIllegal[r][c] = 1;
    }

    checkLines(r,c,height);

    turn = !turn;
    ++turns;

}

bool State::done() const {
    return turns == 64;
}

int State::winner() const {
    if(score[0] == score[1]) return 2;
    return score[0] > score[1] ? 0 : 1;
}