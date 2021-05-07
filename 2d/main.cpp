#include <iostream>

#include "MonteCarlo.hpp"
#include "State.hpp"

using namespace std;

const int ITERATIONS = 100000;

void run();

int main() {

    run();

}

void run() {

    int start, mv;

    cout << "Do you want to go first (0) or second (1)? ";
    cin >> start;

        // cerr << "> 01\n";
    State curState;
        // cerr << "> 02\n";

    if(start == 0) {
        cout << "Make your move: ";
        cin >> mv;
        curState.makeMove(mv);

        cout << endl;
        cout << "Player 0 score: " << curState.score[0] << endl;
        cout << "Player 1 score: " << curState.score[1] << endl;
        cout << endl;
    }

    while(!curState.done()) {

        // cerr << "> 0\n";
        cout << "AI's move: ";
        // cerr << "> 1\n";
        MonteCarlo mcts(curState);
        // cerr << "> 2\n";
        for(int j=0; j<ITERATIONS; ++j) {
            // cerr << "iteration " << j << endl;
            mcts.iterate();
        }

        int aiMv = mcts.bestMove();
        cout << aiMv << endl;
        curState.makeMove(aiMv);

        cout << endl;
        cout << "Player 0 score: " << curState.score[0] << endl;
        cout << "Player 1 score: " << curState.score[1] << endl;
        cout << endl;

        if(curState.done()) break;

        cout << "Make your move: ";
        cin >> mv;
        while(!curState.isLegal(mv)){
            cout << "Illegal move. Try again: ";
            cin >> mv;
        }
        curState.makeMove(mv);

        cout << endl;
        cout << "Player 0 score: " << curState.score[0] << endl;
        cout << "Player 1 score: " << curState.score[1] << endl;
        cout << endl;
    }

    int winner = curState.winner();
    if(winner == 2)
        cout << "It's a draw!\n";
    else 
        cout << "Player " << winner << " wins!\n";

}