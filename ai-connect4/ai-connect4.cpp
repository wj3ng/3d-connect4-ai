#include "STcpClient.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <cstring>
#include <cmath>

using namespace std;

// ========== CONSTANTS ===========

const array<pair<int, int>, 42> index2D{ { // 1-24, out of bounds: 30-41
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
} };

const array<array<int, 3>, 13> dirs{ {
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
} };

typedef array<array<int, 6>, 6> arr66;

// ========== CLASS DEFINITIONS ===========

// Game state
class State {
public:
    int turns; // amount of turns
    int turn; // 0 for black, 1 for white
    int linesMade;
    array<int, 2> score; // 0 for black, 1 for white
    arr66 mask;
    array<arr66, 2> stones; // 0 for black, 1 for white
    vector<int> legalMoves; // list of all legal moves
    arr66 isIllegal;

    State();
    State(const State& G);
    ~State() {
        //array<int, 2>().swap(score);
        //arr66().swap(mask);
        //array<arr66, 2>().swap(stones);
        //vector<int>().swap(legalMoves);
        //arr66().swap(isIllegal);
    }

    bool isLegal(int pos) const;
    void checkLines(int r, int c, int h);
    void makeMove(int pos);
    bool done() const; // true if game is finished
    int winner() const; // 0: black, 1: white, 2: draw 
};

// MCNode with static memory
class MCNodeS {
public:
    State state;
    int ind, parent;
    int expNum;
    array<int, 25> expanded; // 0 if empty
    int totalReward;
    int wonReward;
};

// Monte Carlo Search Tree class using a static array
class MonteCarloS {
public:
    int size;
    array<MCNodeS, 150005> tr;

    MonteCarloS();
    MonteCarloS(const State& S);

    void select(int& cur);
    void expand(int& cur);
    int simulate(int& cur);
    void backtrack(int& cur, int winner);

    void iterate(); // complete one iteration
    int bestMove(); // find best move after root
};


// ========== METHOD IMPLEMENTATIONS ===========


// check if coordinate is within stones
bool isInbounds(int r, int c, int h) {
    return r >= 0
        && r <= 5
        && c >= 0
        && c <= 5
        && h > 0
        && h < (1 << 7);
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
    for (int i = 1; i <= 24; ++i) {
        legalMoves[i - 1] = i;
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
    for (int i = 0; i < 13; ++i) {

        int bef = 0, aft = 0;
        for (int j = 1; j <= 3; ++j) {
            int nr = r + j * dirs[i][0];
            int nc = c + j * dirs[i][1];
            int nh = (dirs[i][2] > 0) ? h << (j * dirs[i][2]) : h >> (j * (-dirs[i][2]));
            if (isInbounds(nr, nc, nh) && (stones[turn][nr][nc] & nh))
                ++aft;
            else
                break;
        }
        for (int j = 1; j <= 3; ++j) {
            int nr = r - j * dirs[i][0];
            int nc = c - j * dirs[i][1];
            int nh = (dirs[i][2] > 0) ? h >> (j * dirs[i][2]) : h << (j * (-dirs[i][2]));
            if (isInbounds(nr, nc, nh) && (stones[turn][nr][nc] & nh))
                ++bef;
            else
                break;
        }

        for (int j = 3; j <= bef + aft; ++j)
            score[turn] += 100 / ++linesMade;

    }
}

// Pre: pos is legal
void State::makeMove(int pos) {

    int r = index2D[pos].first, c = index2D[pos].second;
    int height = mask[r][c] + 1;

    mask[r][c] |= height;
    stones[turn][r][c] |= height;

    if (height == 1 << 6) { // if pole is filled
        for (int i = 0; i < legalMoves.size(); ++i) {
            if (legalMoves[i] == pos)
                legalMoves.erase(legalMoves.begin() + i);
        }
        isIllegal[r][c] = 1;
    }

    checkLines(r, c, height);

    turn = !turn;
    ++turns;

}

bool State::done() const {
    return turns == 64;
}

int State::winner() const {
    if (score[0] == score[1]) return 2;
    return score[0] > score[1] ? 0 : 1;
}


const double EXP_PARAM = sqrt(2); // exploration parameter for ucb1

// initialize + seed randomizer
unsigned timeSeed = chrono::system_clock::now().time_since_epoch().count();
mt19937 mt(timeSeed);

// upper confidence bound 1 formula
double ucb1(int wi, int si, int sp) { // wins, total reward, parent total reward 
    if (si == 0.) return 0.;
    double exploit = (double)wi / si;
    double explore = EXP_PARAM * sqrt(log(sp) / si);
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

    while (!tr[cur].state.done() && tr[cur].expNum == tr[cur].state.legalMoves.size()) {

        double maxUcb1 = 0.;
        int next = 0;

        for (int i : tr[cur].expanded) {
            if (!i) continue;
            double curUcb1 = ucb1(tr[i].wonReward, tr[i].totalReward, tr[i].totalReward);
            if (curUcb1 > maxUcb1) {
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
    for (int i = 1; i <= 24; ++i) {
        if (tr[cur].state.isLegal(i) && !tr[cur].expanded[i])
            unexpanded.push_back(i);
    }

    if (tr[cur].state.done()) return;

    int newMove = unexpanded[mt() % unexpanded.size()];

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

    while (!curState.done()) {
        int newMove = curState.legalMoves[mt() % curState.legalMoves.size()];
        curState.makeMove(newMove);
    }

    return curState.winner();

}

// update simulation results from current node to root
void MonteCarloS::backtrack(int& cur, int winner) {

    while (cur) {

        tr[cur].totalReward += 2;
        if (winner == 2) tr[cur].wonReward += 1;
        else if (winner != tr[cur].state.turn) tr[cur].wonReward += 2;

        cur = tr[cur].parent;

    }

    // update root
    tr[0].totalReward += 2;
    if (winner == 2) tr[0].wonReward += 1;
    else if (winner != tr[0].state.turn) tr[0].wonReward += 2;

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
    for (int i = 1; i <= 24; ++i) {
        if (tr[0].expanded[i] && tr[tr[0].expanded[i]].totalReward > maxReward) {
            ret = i;
            maxReward = tr[tr[0].expanded[i]].totalReward;
        }
    }
    return ret;
}


/*
    輪到此程式移動棋子
    board : 棋盤狀態(vector of vector), board[l][i][j] = l layer, i row, j column 棋盤狀態(l, i, j 從 0 開始)
            0 = 空、1 = 黑、2 = 白、-1 = 四個角落
    is_black : True 表示本程式是黑子、False 表示為白子

    return Step
    Step : vector, Step = {r, c}
            r, c 表示要下棋子的座標位置 (row, column) (zero-base)
*/

State s;
const int ITERATIONS = 150000;
MonteCarloS *mcts;

std::vector<int> GetStep(std::vector<std::vector<std::vector<int>>>& board, bool is_black)
{
    
    // check if new game
    for (int i = 1; i <= 24; ++i) {
        int r = index2D[i].first, c = index2D[i].second;
        if (!board[0][r][c] && (s.mask[r][c] > 1)) {
            s = State();
            break;
        }
    }

    // find last step
    int lastStep = 0;
    for (int i = 1; i <= 24; ++i) {
        int r = index2D[i].first, c = index2D[i].second;

        int height = s.mask[r][c] + 1, h;
        for (h=-1; height>>=1; ++h); // find lg(height)
        if (h < 6 && board[h][r][c]) {
            lastStep = i;
            break;
        }
    }

    if (lastStep) {
        s.makeMove(lastStep);
    }
    
    mcts = new MonteCarloS(s);

    for (int i = 0; i < ITERATIONS; ++i) mcts->iterate();

    int aiMv = mcts->bestMove();
    s.makeMove(aiMv);

    vector<int> ret(2);
    ret[0] = index2D[aiMv].first, ret[1] = index2D[aiMv].second;

    delete mcts;

    return ret;

}


int main()
{
    int id_package;
    std::vector<std::vector<std::vector<int>>> board;
    std::vector<int> step;

    bool is_black;

    while (true)
    {
        if (GetBoard(id_package, board, is_black))
            break;

        step = GetStep(board, is_black);
        SendStep(id_package, step);
    }
}
