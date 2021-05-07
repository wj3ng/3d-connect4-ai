#include <iostream>
#include <chrono>

#include "MonteCarlo.hpp"
#include "State.hpp"

using namespace std;

const int ITERATIONS = 10000;
const int MAX_MS = 4800;

void run();

int main() {

    run();

}

void run() {

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    int start, mv;

    cout << "Do you want to go first (0) or second (1)? ";
    cin >> start;

    State curState;

    if(start == 0) {
        cout << "Make your move: ";
        cin >> mv;
        curState.makeMove(mv);

        cout << endl;
        cout << "--- After " << curState.turns << " turns ---" << endl;
        cout << "Player 0 score: " << curState.score[0] << endl;
        cout << "Player 1 score: " << curState.score[1] << endl;
        cout << endl;
    }

    while(!curState.done()) {

        cout << "AI's move: ";
        MonteCarlo mcts(curState);
        // for(int j=0; j<ITERATIONS; ++j) {
        //     mcts.iterate();
        // }

        auto t1 = high_resolution_clock::now();
        auto t2 = high_resolution_clock::now();
        int iterations = 0;
        while (duration_cast<milliseconds>(t2 - t1).count() < MAX_MS) {
            mcts.iterate();
            ++iterations;
            t2 = high_resolution_clock::now();
        }
        cerr << iterations << " iterations in " << duration_cast<milliseconds>(t2 - t1).count() << " ms. " << endl;


        int aiMv = mcts.bestMove();
        cout << aiMv << endl;
        curState.makeMove(aiMv);

        cout << endl;
        cout << "--- After " << curState.turns << " turns ---" << endl;
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
        cout << "--- After " << curState.turns << " turns ---" << endl;
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