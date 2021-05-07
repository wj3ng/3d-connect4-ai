#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <array>
using namespace std;

typedef array<int,8> arr66;

class State{
public:
    int turn; // 0 for black, 1 for white
    int linesMade;
    array<int, 2> score; // 0 for black, 1 for white
    arr66 mask;
    array<arr66, 2> stones; // 0 for black, 1 for white
    vector<int> legalMoves; // list of all legal moves
    arr66 isIllegal;

    State();
    State(const State& G);

    bool isLegal(int pos) const;
    void checkLines(int p, int h); 
    void makeMove(int pos);
    bool done() const; // true if game is finished
    int winner() const; // 0: black, 1: white, 2: draw 
};

#endif