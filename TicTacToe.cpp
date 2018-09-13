//  Project: Tic-Tac-Toe
//  File name: TicTacToe.cpp
//  Version: 1.0
//  Description: AI algorithm implemented TIC TAC TOE game.
//  Features: 1. Dynamic size of board, try different ROW and COL!
//            2. AI VS AI, AI VS Player,
//            3. Free selection on markers, player hint
//            4. Difficulty selection: easy, medium, hard.
//            5. Simulator added! One time AI vs AI is not cool?
//               Try simulatror now! Run up to 4000 times of AI vs AI games with different depths and check out the result.
//  What's New in 1.0:
//      Enhanced AI VS AI mode, run up to 4000 simulations and get results of win/loss ratio between two different AI depths (smartness).
//      Dynamic display of input range.
//      Bug fixes: Unwanted inputs no longer crash the program, restricted to numbers only.
//
//  Author: Yunho Cho, Julie Shin, Zijian Zhao.
//  Email: jack_zhao98@outlook.com
//  Last updated: Sep 9, 20:53
//  Copyright © 2018 Jack Zhao. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
//  Universal Constants
const int ROW = 3;              // Dimensions of the gameboard
const int COL = ROW;
const int TURN = ROW * COL;     // Most turn possible.
const int winCondition = 3;     // Winning conidtion, can be set to any number of connected markers.
const int X = -1;               // define X as -1 and O as 1
const int O = 1;
//  Depths for AI player, number size determines how "smart" it is.
int minDepth = 2;
int midDepth = 4;
int maxDepth = 8;
int bestMove = 0;   // Best move for the next step.
int boardMarker[ROW][COL];  // 2D array of game status, also controls the board display.

void gameMenu (int & gameMode, bool & humanFirst, int & diffculty);  //  Display game menu, as for game mode.
void gameStart (int gameMode, bool humanTurn, int gameDifficulty);    //  Game start, run the game based on game mode selection.
int setDifficutly (int gameDifficulty);
int numberInput ();     // This function validates the input to make sure that it is a number.

//************* Chess Game Delegate ***********************//
// DEFINE  -1 as X, 1 as O, 10*Row as ? (hint), 0 as empty.
void printBoard ();     //  Print the board according to the boardMarker array.
int askInput ();        //  Ask for user input a position in number.
void play (int position, int currentTurn);          //  Place the mark on a certain position.
void resetBoard();      //  Reset the board to initial state. Change all numbers in the array into 0.

int isOver ();          //  Determine game status: Win/Lose/Draw
int findWinner ();      //  Nested in isOver. Help isOver function to find winner.
//*******   Alignment check, nested in findWinner() *******//
int horAlign ();        //  Horizontal alignment check
int vertAlign ();       //  Vertical alignment check
int diagAlign ();       //  Diagnal alignment check.

bool predictDraw (int humanMark);   //  Predict draw games before the human player filling the board.
bool isFull ();     //  Determine if the board is full.
bool isEmpty ();    //  Determine if the board is empty.
bool isOccupied (int position); // Check if a position is empty or taken.
char askRestart();  //  Ask the user if wish to restart the game.
/************* Chess Game Delegate ENDS ***********************/

/************************** Simulator  *******************************/
void setSimulator (int & maxTime, int & depthOne, int & depthTwo);  // Configuration of simulator, including times and depths.
void runSimulator ();   //  Run simulator.

/*************************  RNG HINT  **************************************/
int RNG ();     //  Random number generator on avalible moves.

/*************************  AI HINT  **************************************/
void getMoves (vector<int> &availableMoves);    //  Gets all available moves on the board.
void getCloseMoves (vector<int> &closeMoves);   //  Get moves only around existing markers.
void revokeTurn (int position);                 //  Undo the turn to restore the chessboard.
int score (int currentPlayer);                  //  Evalutating future moves.
int Adam (int currentPlayer, int depth, int difficulty);    //  Adam, AI player, negamax algorithm implemented.

/****************************  FUNCTION ENDS **********************************/


void gameMenu (int & gameMode, bool & humanFirst, int & diffculty) {
    bool badInput = false;
    int playerMarker = 0;
    cout << "Welcome to Tic-Tac-Toe!\nUse number 1 - " <<  TURN << " to input marks, press 0 for hint.\n\n";
    resetBoard();   //  Reset board status
    printBoard();   //  Print board
    while ((gameMode != 1 && gameMode != 2 && gameMode != 3) || badInput) {
        cout << "Choose game mode:\n  [1] AI VS AI\n  [2] Player VS AI\n  [3] AI VS AI Simulation\n";
        gameMode = numberInput();   //  Game mode selection.
    }
    switch (gameMode) {             //  Only if the game mode is 2, let player to choose markers.
        case 1:
            break;
        case 2:
            while ((playerMarker != 1 && playerMarker !=2) || badInput) {
                cout << "Choose your marker:\n  [1] O\n  [2] X\n";
                playerMarker = numberInput();
            }
            if (playerMarker == 2) {        // Assign human markers.
                humanFirst = true;          //  Determine if human is first hand
                playerMarker = X;
            } else {
                humanFirst = false;
            }
            while ((diffculty != 1 && diffculty != 2 && diffculty != 3) || badInput) {
                cout << "Choose game difficulty:\n  [1] Easy AS Fk\n  [2] Medium\n  [3] Hard Mode\n";
                diffculty = numberInput();  //  Allow player to choose difficulty.
            }
        case 3:
            break;
        default:
            break;
    }
}


void gameStart (int gameMode, bool humanTurn, int gameDifficulty) {

    int hint = 10*ROW;      //  Define hint number as ten times of the row number.
    int humanMark = 0;
    int aiMark = 0;
    int difficulty = 0;
    bool isEnd = false;     //  Game end delegate. true to break the loop.
    if (humanTurn) {        //  Assign markers, if humanTurn is true (firsthand), human is X.
        humanMark = X;
        aiMark = O;
    } else {
        humanMark = O;
        aiMark = X;
    }
    resetBoard();
    printBoard();
    difficulty = setDifficutly(gameDifficulty);
    while (!isFull() && !isEnd) {
        switch (gameMode) {
            case 1:         //  In AI Vs AI, it doesn't matter if humanFirst is true, make it default.
                if (humanTurn) {
                    if (isEmpty()) {        //  If the board is empty, play a random place.
                        play(RNG(), humanMark);
                        printBoard();
                    } else {
                        Adam(humanMark, maxDepth, maxDepth);
                        play(bestMove, humanMark);
                        printBoard();
                    }
                    humanTurn = false;      //  Switch side.
                } else {
                    Adam(aiMark, maxDepth, maxDepth);
                    play(bestMove, aiMark);
                    printBoard();
                    humanTurn = true;
                }
                break;
            case 2:
                if (humanTurn) {
                    if (predictDraw(humanMark)) {       //  Predict draw, if true then end the game.
                        cout << "X O DRAW!\n";
                        printBoard();
                        isEnd = true;
                        break;
                    } else {
                        int move = askInput();
                        if (move == 0) {                //  If user input 0, ask for hint.
                            Adam(humanMark, maxDepth, maxDepth);
                            play(bestMove, hint);
                            printBoard();
                        } else {
                            play(move, humanMark);
                            printBoard();
                            humanTurn = false;
                        }
                    }
                } else {
                    Adam(aiMark, difficulty, difficulty);
                    play(bestMove, aiMark);
                    printBoard();
                    humanTurn = true;
                }
                break;
            case 3:                 //  Run simulator.
                runSimulator();
                isEnd = true;
                break;
            default:
                break;
        }
        if (gameMode != 3) {        //  Only print the result as following when the mode is not simulator (code: 3)
            switch (isOver()) {
                case 0:
                    cout << "X-O DRAW!\n\n";
                    printBoard();
                    isEnd = true;
                    break;
                case O:
                    cout << "\nO WINNER!\n\n";
                    printBoard();
                    isEnd = true;
                    break;
                case X:
                    cout << "\nX WINNER!\n\n";
                    printBoard();
                    isEnd = true;
                    break;
            }
        }
    }
}


void printBoard () {
    for (int i = 0; i < ROW; i ++) {
        for (int j = 0; j < COL; ++j) {
            switch (boardMarker[i][j]) {
                case 0:
                    cout << "   ";
                    break;
                case 10*ROW:
                    cout << " ? ";      //  Print ? as hint place when the code is 10 times of Row#/
                    break;
                case O:
                    cout << " O ";
                    break;
                case X:
                    cout << " X ";
                    break;
                default:
                    break;
            }
            if (j < COL - 1)
                cout << "|";
            else {}
        }
        if (i < ROW - 1) {              //  Dynamic board size corresponding to Column number
            cout << endl;
            for (int j = 0; j < COL; j ++) {
                cout << "----";
            }
            cout << endl;
        } else {
            cout << "\n\n\n";
        }
    }
}

void resetBoard() {
    for (int i = 0; i < ROW; i ++) {
        for (int j = 0; j < COL; j ++) {    // Change all elements in the array to 0 to reset the board.
            boardMarker[i][j] = 0;
        }
    }
}

int setDifficutly (int gameDifficulty) {    // Set difficulty.
    switch (gameDifficulty) {               // Give back different depths based on the game level.
        case 1:
            return (minDepth);
            break;
        case 2:
            return (midDepth);
            break;
        case 3:
            return (maxDepth);
        default:
            break;
    }
    return 0;
}

//  This function will setup some basic parrameters for the simulator, including depths of two AIs, max runs.
void setSimulator (int & maxTime, int & depthOne, int & depthTwo) {
    int timeUpperBound = 4000;     //   Initialize a MAX RUN #.
    int depthUpperBound = maxDepth;
    cout << "Set simulation times: ";
    maxTime = numberInput();
    if (ROW >= 4) {
        timeUpperBound /= 2;        //  Reduce max simulation time to save the CPU when the board is bigger than 4*4.
        depthUpperBound = midDepth; //  Reduce depth, too.
    }
    while (maxTime <= 0 || maxTime > timeUpperBound) {
        cout << "Please enter a number between 0 to " << timeUpperBound << ": ";
        maxTime = numberInput();
    }
    cout << "Set player X's depth: ";
    depthOne = numberInput();
    while (depthOne <= 0 || depthOne > depthUpperBound) {
        cout << "Save the CPU, set a number between 0 to " << depthUpperBound << ": ";
        depthOne = numberInput();
    }
    cout << "Set player O's depth: ";
    depthTwo = numberInput();
    while (depthTwo <= 0 || depthTwo > depthUpperBound) {
        cout << "Save the CPU, set a number between 0 to " << depthUpperBound << ": ";
        depthTwo = numberInput();
    }
}

void runSimulator () {
    
    int xWin = 0;   //  These are the match records, increment by one if win condition has been reached.
    int oWin = 0;
    int draw = 0;
    int depthOne = 0;
    int depthTwo = 0;
    int maxTime = 0;
    bool playerOne = true;
    
    setSimulator(maxTime, depthOne, depthTwo);
    
    for (int i = 1; i <= maxTime; i ++) {
        bool isEnd = false;
        resetBoard();
        while (!isFull() && !isEnd) {
            if (playerOne) {
                if (isEmpty()) {
                    play(RNG(), X);
                } else {
                    Adam(X, depthOne, depthOne);
                    play(bestMove, X);
                }
                playerOne = false;      //  Switch side.
            } else {
                Adam(O, depthTwo, depthTwo);
                play(bestMove, O);
                playerOne = true;       //  Switch side.
            }
            if (isOver() != 2) {
                switch (isOver()) {
                    case X:
                        xWin ++;
                        break;
                    case O:
                        oWin ++;
                        break;
                    case 0:
                        draw = draw + 1;
                        break;
                    default:
                        break;
                }
                isEnd = true;
            }
        }
        if (maxTime >= 10) {
            if (i % int(double(maxTime)/10) <= 0.5)
                cout << double(i)/maxTime * 100 << "% Complete." << " (" << i << "/" << maxTime << ")" << endl;
        }
    }
    cout << "\nSimulation is over!" << endl
    << "Total " << maxTime << " times." << endl
    << " -- X wins " << xWin << " games." << endl
    << " -- O wins " << oWin << " games." << endl
    << " -- X O Draw " << draw << " games." << endl;
}

bool predictDraw (int humanMark) {
    vector<int> availableMoves;
    getMoves(availableMoves);                                           //  First, get all available moves on the board.
    while (availableMoves.size() <= 2 && availableMoves.size() > 0) {   // Only if there are two steps left, possible draw game might exist.
        play(availableMoves[availableMoves.size()-1], humanMark);       // Play a specific 'available move position' as Human player.
        if (findWinner() == humanMark) {
            revokeTurn(availableMoves[availableMoves.size()-1]);        //  If human have a chance to win, return false (not draw) then revoke.
            return false;
        } else {
            revokeTurn(availableMoves[availableMoves.size()-1]);
        }
        play(availableMoves[availableMoves.size()-1], -humanMark);      //  Play a specific 'available move position' as AI player.
        if (findWinner() == -humanMark) {
            revokeTurn(availableMoves[availableMoves.size()-1]);
            return false;                                               //  If AI have a chance to win, return false (not draw) then revoke.
        } else {
            revokeTurn(availableMoves[availableMoves.size()-1]);
        }
        availableMoves.pop_back();                                      //  Remove the last item in the array.
        return true;
    }                                                                   //  If no player can reach victory, return true. (draw game predicted)
    return false;
}

int numberInput () {    //  Ask for number input only.
    int number = 0;
    bool notNumber = true;
    while (notNumber) {
        cin >> number;
        notNumber = cin.fail();     // if cin.fail() is true, the while loop continues.
        if (notNumber)
            cout << "Check input, numbers only.\n";
        cin.clear();
        cin.ignore(10,'\n');        // Clear user input.
    }
    return number;
}

int askInput () {       //  Ask for human player's input, numbers only.
    int input = 0;
    do {                //  Loop that will validate the input. Will loop if number exceeds the range, or the place is taken.
        cout << endl << "Your turn! Choose wisely: ";
        input = numberInput();
        if (isOccupied(input))
            cout << "Place has been taken, try again.\n";
        else if (input < 0 || input > TURN)
            cout << "Invalid number, enter number between 1 - " << TURN << "!\n";
    } while (input < 0 || input > TURN || isOccupied(input));
    return input;
}

bool isEmpty () {       //  Check if the entire board is empty, designated to check out if the game has started.
    for (int i = 1; i <= TURN; i ++) {
        if (isOccupied(i)) {
            return false;
        }
    }
    return true;
}

int isOver () {         // Either a winner exists, or the board is full, game is over
    if (findWinner() != 0)
        return findWinner();
    else if (isFull())
        return 0;
    else
        return 2;
}

bool isFull () {           //   If there is no zero in the array, return true.
    for (int i = 0; i < ROW; i ++) {
        for (int j = 0; j < COL; j ++) {
            if (boardMarker[i][j] == 0) {
                return false;
            } else {}
        }
    }
    return true;
}

int findWinner () {         // find winner function. If one of the alignment exists, return winner's code.
    if (horAlign()) {
        return horAlign();
    } else if (vertAlign()) {
        return vertAlign();
    } else if (diagAlign()) {
        return diagAlign();
    } else {                // No alignment, return 0.
        return 0;
    }
}

// Check alignment on Horizontal/Vertical/Diagnal
int horAlign () {
    for (int row = 0; row < ROW; row ++) {          //  Starts from left to right.
        for (int col = 0; col <= COL - winCondition; col ++) {
            int tempSum = 0;
            for (int i = col; i < col + winCondition; i ++) {
                tempSum += boardMarker[row][i];
            }
            if (abs(tempSum) == winCondition) {      //  If abs(sum) is winCondition, return value.
                if (tempSum < 0) {
                    return X;
                } else {
                    return O;
                }
            }
        }
    }
    return 0;
}

int vertAlign () {
    for (int col = 0; col < COL; col ++) {          //  Starts from top to bottom.
        for (int row = 0; row <= ROW - winCondition; row ++) {
            int tempSum = 0;
            for (int i = row; i < row + winCondition; i ++) {
                tempSum += boardMarker[i][col];
            }
            if (abs(tempSum) == winCondition) {      //  If abs(sum) is winCondition, return value.
                if (tempSum < 0) {
                    return X;
                } else {
                    return O;
                }
            }
        }
    }
    return 0;
}

int diagAlign () {
    //  "\" direction
    for (int row = 0; row <= ROW - winCondition; row ++) {  // Starts from top left to bottom right.
        for (int col = 0; col <= COL - winCondition; col ++) {
            int tempSum = 0;
            for (int i = 0; i < winCondition; i ++) {    //  If abs(sum) is winCondition, return value.
                tempSum += boardMarker[row+i][col+i];
            }
            if (abs(tempSum) == winCondition) {
                if (tempSum < 0) {
                    return X;
                } else {
                    return O;
                }
            }
        }
    }
    //  "/" direction
    for (int row = 0; row <= ROW - winCondition; row ++) {  // Starts from top right to bottom left.
        for (int col = COL-1; col >= winCondition -1; col --) {
            int tempSum = 0;
            for (int i = 0; i < winCondition; i ++) {
                tempSum += boardMarker[row+i][col-i];       //  If abs(sum) is winCondition, return value.
            }
            if (abs(tempSum) == winCondition) {
                if (tempSum < 0) {
                    return X;
                } else {
                    return O;
                }
            }
        }
    }
    return 0;
}
// Control Game
// Place O/X on a certain positoin from 1-9
void play (int position, int currentTurn) {
    int row = (position - 1) / ROW;
    int col = (position - 1) % COL;
    if (!isOccupied(position)) {
        boardMarker[row][col] = currentTurn;
    } else {   }
}
//  Boolean function to find out if a certain spot is taken.
bool isOccupied (int position) {
    int row = (position - 1) / ROW;
    int col = (position - 1) % COL;
    if (boardMarker[row][col] == 0 || boardMarker[row][col] == 10 * ROW) {    // if any spot is empty or marked as hint, it is not occupied.
        return false;
    } else {
        return true;
    }
}
// Get all available moves in an vector array.
void getMoves (vector<int> &availableMoves) {
    for (int i = 1; i <= TURN; i ++) {
        if (!isOccupied(i)) {
            availableMoves.push_back(i);
        } else {}
    }
}
//  Get all available moves around existing markers only.
void getCloseMoves (vector<int> &closeMoves) {
    for (int i = 1; i <= TURN; i ++) {
        int row = (i - 1) / ROW;
        int col = (i - 1) % COL;
        
        //* NOTE *  Assume position around i is like 1 - 9, i is in the middle with 5.
        
        if (!isOccupied(i)) {   // Check positions that are not occupied.
            if (row == 0) {     // if the position is on the top edge,
                
                if (col == 0) { // if the position is also on the left edge
                    if (isOccupied(i+1) || isOccupied(i + COL) || isOccupied(i + COL + 1)) {    // check spot 6 8 9 only.
                        closeMoves.push_back(i);
                    }
                } else if (col == COL - 1) {    // if the position is on the right edge.
                    if (isOccupied(i-1) || isOccupied(i + COL) ||isOccupied(i + COL - 1)) {     // check spot 4 7 8 only.
                        closeMoves.push_back(i);
                    }
                } else {
                    if (isOccupied(i+1) || isOccupied(i-1) || isOccupied(i + COL) || isOccupied(i + COL + 1) || isOccupied(i + COL - 1)) // check 4 6 7 8 9
                        closeMoves.push_back(i);
                }
                
            } else if (row == ROW - 1) {    // positions on the bottom edge.
                if (col == 0) {             // left edge
                    if (isOccupied(i+1) || isOccupied(i - COL) || isOccupied(i - COL + 1)) {    // check 2 3 6
                        closeMoves.push_back(i);
                    }
                } else if (col == COL - 1) {    // right edge.
                    if (isOccupied(i-1) || isOccupied(i - COL) ||isOccupied(i - COL - 1)) {     // check 1 2 4
                        closeMoves.push_back(i);
                    }
                } else {
                    if (isOccupied(i+1) || isOccupied(i-1) || isOccupied(i-COL) || isOccupied(i - COL - 1) || isOccupied(i - COL + 1))
                        closeMoves.push_back(i);    // check 1 2 3 4 6
                }
                
            } else {
                if (col == 0) { // left edge only
                    if (isOccupied(i+1) || isOccupied(i-COL) || isOccupied(i - COL - 1) ||isOccupied(i + COL) || isOccupied(i + COL - 1)) {
                        closeMoves.push_back(i);        // check 2 3 6 8 9
                    }
                } else if (col == COL - 1) {    // right edge only.
                    if (isOccupied(i-1) || isOccupied(i-COL) || isOccupied(i - COL - 1) || isOccupied(i + COL) || isOccupied(i + COL - 1)) {
                        closeMoves.push_back(i);        // check 1 2 4 7 8
                    }
                } else {
                    if (isOccupied(i+1) || isOccupied(i-1) || isOccupied(i-COL) || isOccupied(i - COL - 1) || isOccupied(i - COL + 1) || isOccupied(i + COL) || isOccupied(i + COL + 1) || isOccupied(i + COL - 1)) {
                        closeMoves.push_back(i);    // check 1 2 3 4 6 7 8 9
                    }
                }
            }
        } else {}
    }
}

int RNG () {                            // Random number generator.
    vector<int> availableMoves;
    getMoves(availableMoves);           // Get available moves on the board.
    int i = rand() % availableMoves.size();
    return (availableMoves[i]);
}

void revokeTurn (int position) {            // This function can undo the turn on a certain position.
    int row = (position - 1) / ROW;
    int col = (position - 1) % COL;
    if (boardMarker[row][col] != 0) {
        boardMarker[row][col] = 0;
    } else {}
}

int score (int currentPlayer) {             // Evalutation function
    if (findWinner() == O) {                // To current player, the best result is our victory, (score as high as possible)
        return 10000 * currentPlayer;       // therefore, if the rival wins the game will turn out a negative number.
    } else if (findWinner() == X){
        return -10000 * currentPlayer;
    } else {
        return 0;
    }
}

int Adam (int currentPlayer, int depth, int difficulty) {
    int bestScore = -1000000;               // Initialize a low score of our customer/ the maximizing player.
    // The goal is to get the best move when the score is as high as possible.
    if (depth == 0 || isOver() != 2) {      // When the depth goes 0, or the game is over,
        return score(currentPlayer);        // return as a evaluation score.
    } else {
        vector<int> futureSteps;
        //getMoves(futureSteps);            // This is no longer used in AI function as the moves are too many as the board grows bigger.
        getCloseMoves(futureSteps);         // Instead, use new function to get steps only around existing markers to reduce time.
        while (futureSteps.size() > 0) {    // Only repeat the loop when there are unchecked available spots.
            int move = futureSteps[futureSteps.size() - 1]; // Reduce the size by one after checking a specific spot.
            play(move, currentPlayer);      // Simulate moves.
            int newScore = -Adam(-currentPlayer, depth - 1, difficulty);    // New score equals to the negative score of the other player.
            // This algorithm is based on Max(a,b) is -Min(a,b). New score will be compared with existing score, and keep the biggest. The other player's largest negation is what current player wants to keep.
            //cout << newScore << "   " << bestScore << endl;
            futureSteps.pop_back();         // Delete the last item in the vector array.
            revokeTurn(move);               // revoke the simulated move to restore chessboard.
            // In each loop, we are searching for the biggest value of the negation of the othe player.
            if (newScore >= bestScore) {    // if the newscore is bigger than the best score,
                bestScore = newScore;       // set the new score as the best score since the maximizing player need the move when the score is at its highest.
                if (depth == difficulty) {  // record the best move when the depth is back to the first node.
                    bestMove = move;
                }
            }
        }
    }
    return bestScore;
}

char askRestart() {
    char choice = 'n';
    cout << "One more game? Y/N (N): ";
    cin >> choice;
    while (choice != 'n' && choice != 'N' && choice != 'y' && choice != 'Y') {
        cout << "Try again!\n";
        cout << "One more game? (Y/N): ";
        cin >> choice;
    }
    return choice;
}

int main() {
    srand(time(NULL));          //  generate seed to random different place.
    bool humanFirst = true;
    char restart = 'y';
    while (restart == 'y' || restart == 'Y') {
        int gameMode = 0;
        int gameDifficulty = 0;
        gameMenu(gameMode, humanFirst, gameDifficulty);
        gameStart(gameMode, humanFirst, gameDifficulty);
        restart = askRestart();
    }
    return 0;
}

