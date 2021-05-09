//
//  main.cpp
//  Chess game
//
//  Created by Craig Kogan on 2020-02-02.
//  Copyright © 2020 Craig Kogan. All rights reserved.
//

#include <iostream>
#include <vector>
#include <ctime>
#include <string>
using namespace std;

//Definition of the size of the table - must be done here so the display function works properly.
int const X(9);
int const Y(9);
int const maxMoves(2); //Change this value to alter the amount of moves ahead that the AI looks. NOTE THAT ANY VALUE GREATER THAN THREE WILL CAUSE EXTREME LAG. THERE IS A SMALL LAG TIME WHEN SET AT 3.
//When set to 0, AI will only look at it's next move. <- DOES NOT DO THIS. ALWAYS CHECKS NEXT WHITE
//When set at 0, AI will look at it's next move and White's potential next move.
//When set at 1, AI will look at it's next move, White's potential next move as well as it's next potential move.
//Pattern continues as described above.
//RECOMENDED SETTING FOR VERY MINIMAL/NO LAG: 2

//Function to display the board
//This function is also used to count the number of Kings still alive - if there are fewer than 2 kings alive, the game ends.
int displayBoard(int board[X][Y], int ownership[X][Y]) {
    int kingCount(0);
    cout << endl;
    for (int li(0); li < X; li++) {
    for (int col(0); col < Y; col++) {
        //Top left corner - singularity
        if (li == 0 && col == 0)
        {
            cout << char(board[li][col]);
        }
        //Top line (letter coordinate) - line singularity
        else if (li == 0 && col != 0)
        {
            cout << "  ";
            cout << char(board[li][col]);
        }
        //Left line (number coordinate) - column singularity
        else if (li != 0 && col == 0)
        {
            cout << char(board[li][col]);
        }
        //Pieces
        else if (li != 0 && col != 0)
        {
            cout << "| ";
            //White team pieces - capital letters
            if (ownership[li][col] == 'W') {
                cout << char(toupper(board[li][col]));
            }
            //Black team pieces - lowercase letters
            else if (ownership[li][col] == 'B'){
                cout << char(tolower(board[li][col]));
            }
            //Empty tiles
            else {
                cout << char(board[li][col]);
            }
        }
        //Identify if both kings are still alive - used to end the game
        if (board[li][col] == int(75))
        {
            kingCount++;
        }
        cout << " ";
    }
    //End of line singularity accomodation
    if (li != 0)
    {
        cout << "|";
        cout << char(board[li][0]);
    }
    cout << endl;
    cout << "  +---+---+---+---+---+---+---+---+" << endl;
}
    cout << " ";
    for (int col(1); col < Y; col++) {
        cout << "   ";
        cout << char(board[0][col]);
    }
    cout << "\n" << endl;
    return kingCount;
}

//Asks the player where they want to move their selected piece.
string movePiece() {
    cout << "Identify the tile that you would like to move the piece to. Always enter the letter before the number. (Example: E5)." << endl;
    string move;
    cin >> move;
    return move;
}

//The movement legality check for the Pawns. Placed in a function due to the large number of conditions.
int movePawn(int playerKey, int tileSelectLi, int tileSelectCol, int moveLi, int moveCol, int ownership[X][Y], string ghostPawnWh, string ghostPawnBl) {
    int legalPawn(0);
    //Pawn movement structure
    //WHITE
    if (playerKey == 87 && tileSelectLi > moveLi)
    {
        //Pawn's first move
        if (tileSelectLi == 7 && (abs(tileSelectLi - moveLi) == 1 || abs(tileSelectLi - moveLi) == 2) && abs(tileSelectCol - moveCol) == 0 && ownership[moveLi][moveCol] == 32)
        {
            legalPawn = 1;
        }
        //All other moves
        else if (abs(tileSelectLi - moveLi) == 1)
        {
            int ghostLi(ghostPawnBl[1] - 49);
            int ghostCol(toupper(ghostPawnBl[0]) - 64);
            //Standard move
            if (ownership[moveLi][moveCol] == 32 && abs(tileSelectCol - moveCol) == 0)
            {
                legalPawn = 1;
            }
            //Capturing a piece
            else if (ownership[moveLi][moveCol] == 66 && abs(tileSelectCol - moveCol) == 1)
            {
                legalPawn = 1;
            }
            //En passent
            else if (moveLi == ghostLi && moveCol == ghostCol)
            {
                legalPawn = 2;
            }
        }
    }
    //BLACK
    else if (playerKey == 66 && tileSelectLi < moveLi)
    {
        //Pawn's first move
        if (tileSelectLi == 2 && (abs(tileSelectLi - moveLi) == 1 || abs(tileSelectLi - moveLi) == 2) && abs(tileSelectCol - moveCol) == 0 && ownership[moveLi][moveCol] == 32)
        {
            legalPawn = 1;
        }
        //All other moves
        else if (abs(tileSelectLi - moveLi) == 1)
        {
            int ghostLi(ghostPawnWh[1] - 47);
            int ghostCol(toupper(ghostPawnWh[0]) - 64);
            //Standard move
            if (ownership[moveLi][moveCol] == 32 && abs(tileSelectCol - moveCol) == 0)
            {
                legalPawn = 1;
            }
            //Capturing a piece
            else if (ownership[moveLi][moveCol] == 87 && abs(tileSelectCol - moveCol) == 1)
            {
                legalPawn = 1;
            }
            //En passent
            else if (moveLi == ghostLi && moveCol == ghostCol)
            {
                legalPawn = 2;
            }
        }
    }
    else
    {
        legalPawn = 0;
    }
    return legalPawn;
}

//Prevents a piece from capturing a piece on the same team.
int sameTeamCapture(int playerKey, int moveLi, int moveCol, int ownership[X][Y]) {
    int STC;
    if (ownership[moveLi][moveCol] == playerKey) {
        STC = 0;
    } else {
        STC = 1;
    }
    return STC;
}

//Registers capturing a piece on the opposing team
//Used for AI move descision structure.
int enemyTeamCapture(int playerKey, int moveLi, int moveCol, int ownership[X][Y], int board[X][Y]) {
    int ETC;
    int oppKey;
    if (playerKey == 87) {
        oppKey = 66;
    }
    else {
        oppKey = 87;
    }
    
    if (ownership[moveLi][moveCol] == oppKey) {
        if (board[moveLi][moveCol] == int(82)) { //rook
            ETC = 50; }
        else if (board[moveLi][moveCol] == int(78)) { //knight
            ETC = 30; }
        else if (board[moveLi][moveCol] == int(66)) { //bishop
            ETC = 30; }
        else if (board[moveLi][moveCol] == int(81)) { //queen
            ETC = 90; }
        else if (board[moveLi][moveCol] == int(75)) { //king
            ETC = 1000; }
        else if (board[moveLi][moveCol] == int(80)) { //pawn
            ETC = 10; }
        else {
            ETC = 20; }
        }
    else {
    ETC = 0; }
    
    if (playerKey == 87) {
        ETC = ETC*-1; }
    
    return ETC;
}

//'clears' the screen by skipping down 50 lines. An aesthetic feature.
void clearScreen() {
cout << string(50,'\n');
}

//Ensure that a piece is not bypassing another piece during movement
int movementCheck(int playerKey, int tileSelectLi, int tileSelectCol, int moveLi, int moveCol, int ownership[X][Y], int STC, int AI) {
    //extra condition to ensure that the piece is not moving outside of the board
    //added as an extra security measure for the AI system as the player inputed commands have to pass this test earlier.
    if (moveLi > int(8) || moveLi < int(1) || moveCol > int(8) || moveCol < int(1)) {
        int valid(0);
        return valid;
    }
    //This is an optimization measure. If STC is failed, the movementCheck is uneeded.
    if (STC == 0) {
        int valid(0);
        return valid;
    }
    //ONLY the AI goes to this part. Essentially, since the AI checks square by square, if all previous squares were good, then the previous squares do not need to be checked (they were checked by the STC checks one at a time as the piece checked its moves along the line). Therefore, the AI skips the other checks.
    //THIS OPTIMIZES THE CODE -> this would work ONLY if ETC is also checked. WIP
    /*
    if (AI == 1) {
        int valid(1);
        return valid;
    }
    */
    int valid(1);
    //Determine direction
    char horzMove('N');
    char vertMove('N');
    //Left
    if ((tileSelectCol - moveCol) > 0) {
        horzMove = 'L';
    }
    //Right
    else if ((tileSelectCol - moveCol) < 0) {
        horzMove = 'R';
    }
    //Up
    if ((tileSelectLi - moveLi) > 0) {
        vertMove = 'U';
    }
    //Down
    else if ((tileSelectLi - moveLi) < 0) {
        vertMove = 'D';
    }
    
    //diagonal is checked first since a diagonal move is technically both a horizontal and a verticle movement
    //Diagonal movement
    if (abs(tileSelectLi - moveLi) != 0 && abs(tileSelectCol - moveCol) != 0)
    {
        //this condition prevents any irregular movement that is not a clean diagonal
        if (abs(tileSelectLi - moveLi) != abs(tileSelectCol - moveCol))
        {
            valid = 0;
            return valid;
        }
        else if (horzMove == 'L' && vertMove == 'U') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi - move][tileSelectCol - move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
        else if (horzMove == 'L' && vertMove == 'D') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi + move][tileSelectCol - move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
        else if (horzMove == 'R' && vertMove == 'U') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi - move][tileSelectCol + move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
        else if (horzMove == 'R' && vertMove == 'D') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi + move][tileSelectCol + move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
    }
    //Horizontal movement
    else if (abs(tileSelectCol - moveCol) != 0 && abs(tileSelectLi - moveLi) == 0)
    {
        if (horzMove == 'L') {
            for (int move(1); move < abs(tileSelectCol - moveCol); move++) {
                if (ownership[tileSelectLi][tileSelectCol - move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
        else if (horzMove == 'R') {
            for (int move(1); move < abs(tileSelectCol - moveCol); move++) {
                if (ownership[tileSelectLi][tileSelectCol + move] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
    }
    //Vertical movement
    else if (abs(tileSelectCol - moveCol) == 0 && abs(tileSelectLi - moveLi) != 0)
    {
        if (vertMove == 'U') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi - move][tileSelectCol] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
        else if (vertMove == 'D') {
            for (int move(1); move < abs(tileSelectLi - moveLi); move++) {
                if (ownership[tileSelectLi + move][tileSelectCol] != int(32)) {
                    valid = 0;
                    return valid;;
                }
            }
        }
    }
    return valid;
}

//Function that displays the illegal move message.
int illegalMoveText() {
    clearScreen();
    cout << "\n\nThat is an illegal move! Take another turn. " << endl;
    return 0;
}

//Function to see if the opposing team is in check
//This function works by doing a mock movement of the piece that was just moved and attempting to move it to the position of the opposing king.
//If this returns a value registering if the opposing team is in check, then I just need to do a check to make sure that said opposing team is no longer in check after their next move.
int inCheckTest(int playerKey, int moveLiOld, int moveColOld, int ownership[X][Y], string whKingPos, string blKingPos, int turnRotation, int pieceID) {
    
    int tileSelectLi = moveLiOld;
    int tileSelectCol = moveColOld;
    int moveLi(0);
    int moveCol(0);
    if (playerKey == 87) {
        moveLi = (blKingPos[1] - 48);
        moveCol = (blKingPos[0] - 64);
    }
    else if (playerKey == 66) {
        moveLi = (whKingPos[1] - 48);
        moveCol = (whKingPos[0] - 64);
    }
    //attempt to move the piece (unidentified) to capture the opposing king
    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, 1, 0);
    
    //These are defined for the King's and Pawn's movement attack pattern
    int liTestK = abs(tileSelectLi - moveLi);
    int colTestK = abs(tileSelectCol - moveCol);
    
    //fixes the message
    if (turnRotation == 0) {
        turnRotation = 2;
    }
    
    //Verifies that the piece is moving in a legal way and that it has a clear line to the opposing king (this is done in the movementCheck above)
    //These 6 IF/ELSE IF statements verifies that a piece can only put the king in check if it can attack the king by following it's typical attack movement pattern
    //Rook
    if ((tileSelectCol == moveCol || tileSelectLi == moveLi) && pieceID == int(82) && valid == 1) {
        return 1;
    }
    //Knight
    else if  (((abs(tileSelectLi - moveLi) == 2 && abs(tileSelectCol - moveCol) == 1) || (abs(tileSelectLi - moveLi) == 1 && abs(tileSelectCol - moveCol) == 2)) && pieceID == int(78)) {
        return 1;
    }
    //Bishop
    else if((abs(tileSelectLi - moveLi) == abs(tileSelectCol - moveCol)) && pieceID == int(66) && valid == 1) {
        return 1;
    }
    //Queen
    else if ((tileSelectCol == moveCol || tileSelectLi == moveLi || abs(tileSelectLi - moveLi) == abs(tileSelectCol - moveCol)) && pieceID == int(81) && valid == 1) {
        return 1;
    }
    //King
    else if ((((colTestK == 0 &&  liTestK == 1) || (liTestK == 0 &&  colTestK == 1) || (liTestK == 1 && colTestK == 1)) && pieceID == int(75)) && valid == 1) {
        return 1;
    }
    //Pawn -> a pawn can only attack at a diagonal making it's attack pattern easy
    else if ((liTestK == 1 && colTestK == 1) && pieceID == int(81) && valid == 1) {
        return 1;
    }
    return 0;
}

//AI IMPLEMENTATION BLOCK BELOW

float AiTestingSimulationMove(int board[X][Y], int ownership[X][Y], int playerKey, int movesAhead) {
    
    playerKey = playerKey*-1 +153;
    vector<int> initialPiecePos;
    for (int li(1); li < 9; li++) {
    for (int col(1); col < 9; col++) {
        if (ownership[li][col] == char(playerKey)) {
            initialPiecePos.push_back((li*10 + col)*100);
        }
    }
    }

    vector<int> pieceMoves;
    vector<float> pieceMoveValues;
    
    //Determines ALL possible moves for the designated team.
    //Can be altered (change in playerKey) to determine ALL possible moves for the non-AI team.
    for (int possMoves = 0; possMoves < initialPiecePos.size(); possMoves+=1) {
    int masterLiCopy(int(to_string(initialPiecePos[possMoves])[0]-48));
    int masterColCopy(int(to_string(initialPiecePos[possMoves])[1]-48));
        
    //rook moves
    if (board[masterLiCopy][masterColCopy] == int(82)) {
        for (int directions(0); directions <4; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //left
                    while (colCopy > 0) {
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //up
                    while (liCopy > 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //right
                    while (colCopy < 9) {
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //down
                    while (liCopy < 9) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
        }
    }

    //knight moves
    else if (board[masterLiCopy][masterColCopy] == int(78)) {
        for (int directions(0); directions <8; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            
            if (directions == 0) /*UUL*/ {
                liCopy-=2;
                colCopy-=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 1) /*UUR*/ {
                liCopy-=2;
                colCopy+=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 2) /*DDR*/ {
                liCopy+=2;
                colCopy+=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 3) /*DDL*/ {
                liCopy+=2;
                colCopy-=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 4) /*ULL*/ {
                liCopy-=1;
                colCopy-=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 5) /*URR*/ {
                liCopy-=1;
                colCopy+=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 6) /*DRR*/ {
                liCopy+=1;
                colCopy+=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 7) /*DLL*/ {
                liCopy+=1;
                colCopy-=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }

            }
        }

    //bishop moves
    else if (board[masterLiCopy][masterColCopy] == int(66)) {
        for (int directions(0); directions <4; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //left-up
                    while (liCopy > 0 && colCopy > 0) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //left-down
                    while (liCopy < 9 && colCopy > 0) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //right-up
                    while (liCopy > 0 && colCopy < 9) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //right-down
                    while (liCopy < 9 && colCopy < 9) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
        }
    }

    //queen moves
    else if (board[masterLiCopy][masterColCopy] == int(81)) {
        for (int directions(0); directions <8; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //left
                    while (colCopy > 0) {
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //up
                    while (liCopy > 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //right
                    while (colCopy < 9) {
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //down
                    while (liCopy < 9) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 4: //left-up
                    while (liCopy > 0 && colCopy > 0) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 5: //left-down
                    while (liCopy < 9 && colCopy > 0) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 6: //right-up
                    while (liCopy > 0 && colCopy < 9) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 7: //right-down
                    while (liCopy < 9 && colCopy < 9) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
            }
        }

    //king moves - castle-ing is NOT IMPLEMENTED INTO THE AI's move set.
    else if (board[masterLiCopy][masterColCopy] == int(75)) {
    for (int directions(0); directions <8; directions++) {
    int liCopy = masterLiCopy;
    int colCopy = masterColCopy;
        
        if (directions == 0) /*UL*/ {
            liCopy-=1;
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 1) /*U*/ {
            liCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 2) /*UR*/ {
            liCopy-=1;
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 3) /*R*/ {
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 4) /*DR*/ {
            liCopy+=1;
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
         
        else if (directions == 5) /*D*/ {
            liCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 6) /*DL*/ {
            liCopy+=1;
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 7) /*L*/ {
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }

        }
    }

    //pawn moves - en passent is NOT IMPLEMENTED INTO THE AI's move set.
    else if (board[masterLiCopy][masterColCopy] == int(80)) {
        //black pawns
        if (playerKey == int(66)) {
            //first move - two spaces move
            if (masterLiCopy == int(2)) {
                int liCopy = (masterLiCopy) + 2;
                int colCopy = masterColCopy;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                
                //Add start/end + capture value of Pawn move
                if (STC == 1 && valid == 1 && legalPawn == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    pieceMoveValues.push_back(0);
                }
            }
            
            //all other moves - 1 space move - capture moves
                for (int directions(0); directions <3; directions++) {
                    int liCopy = (masterLiCopy);
                    int colCopy = masterColCopy;
                    //1 space forward
                    if (directions == 0) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            pieceMoveValues.push_back(0);
                        }
                    }
                    //capturing to the left
                    else if (directions == 1) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                    //capturing to the right
                    else if (directions == 2) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
            }
        }
        
        //white pawns
        if (playerKey == int(87)) {
            //first move - two spaces move
            if (masterLiCopy == int(7)) {
                int liCopy = (masterLiCopy) - 2;
                int colCopy = masterColCopy;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                
                //Add start/end + capture value of Pawn move
                if (STC == 1 && valid == 1 && legalPawn == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    pieceMoveValues.push_back(0);
                }
            }
            
            //all other moves - 1 space move - capture moves
                for (int directions(0); directions <3; directions++) {
                    int liCopy = (masterLiCopy);
                    int colCopy = masterColCopy;
                    //1 space forward
                    if (directions == 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            pieceMoveValues.push_back(0);
                        }
                    }
                    //capturing to the left
                    else if (directions == 1) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                    //capturing to the right
                    else if (directions == 2) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                }
        }
    }
    }

    //START WHITE
    if (playerKey == 87) {

        //looks at the next move. Skipped if moves ahead cap is reached.
        if (movesAhead < maxMoves) {
            movesAhead++;
            //goes through each move discovered and moves the piece -> used for looking move(s) ahead.
            for (int i(0); i < pieceMoves.size(); i++) {
            
            string AiMoveSTR = to_string(pieceMoves[i]);
            //extraction
            int tileSelectLi(int(AiMoveSTR[0])-48);
            int tileSelectCol(int(AiMoveSTR[1])-48);
            int moveLi(int(AiMoveSTR[2])-48);
            int moveCol(int(AiMoveSTR[3])-48);
            int pieceID = board[tileSelectLi][tileSelectCol];
            int pieceIDSave = board[moveLi][moveCol];
            int playerKeySave = ownership[moveLi][moveCol];
            
            //movement
            board[tileSelectLi][tileSelectCol] = int(32);
            board[moveLi][moveCol] = pieceID;
            ownership[tileSelectLi][tileSelectCol] = int(32);
            ownership[moveLi][moveCol] = playerKey;
            
            float pieceMoveValuesAddition = AiTestingSimulationMove(board, ownership, playerKey, movesAhead);
            pieceMoveValues[i]+=pieceMoveValuesAddition;
            
            //de-movement
            board[tileSelectLi][tileSelectCol] = pieceID;
            board[moveLi][moveCol] = pieceIDSave;
            ownership[tileSelectLi][tileSelectCol] = playerKey;
            ownership[moveLi][moveCol] = playerKeySave;
            }
        }
        
        //extra checks section (protections, forks) -> pins and forks sohuld be realized due to the looping system.
        //eleminates all non-pawn capture moves from the list -> used for protection/pin/fork tests
        vector<int> pieceMovesPawnsAttack;
        for (int i(0); i < pieceMoves.size(); i++) {
            
            string currentSTR = to_string(pieceMoves[i]);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            int moveCol(int(currentSTR[3])-48);
            int pieceID = board[tileSelectLi][tileSelectCol];
            
            if (pieceID == 80 && tileSelectCol == moveCol) {
            pieceMovesPawnsAttack.push_back(pieceMoves[i]%100);
            }
            else {
                pieceMovesPawnsAttack.push_back(pieceMoves[i]);
            }
        }
        
        //checks for protection.
        for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
            //Solution for protection on both end of the list -> prevents repeated protection points from stacking
            int passProtect = 0;
            int current = pieceMovesPawnsAttack[i]%100;
                
            //white PP
            string currentSTR = to_string(current);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            //pawns protect!
            if (board[tileSelectLi+1][tileSelectCol-1] == 80) {
                pieceMoveValues[i]-=5;
                continue;
            }
            else if (board[tileSelectLi+1][tileSelectCol+1] == 80) {
                pieceMoveValues[i]-=5;
                continue;
            }
            //next elements in the list
            for (long int i2 = i+1; i2 < pieceMovesPawnsAttack.size(); i2++) {
                if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                    pieceMoveValues[i]-=5;
                    passProtect = 1;
                    break;
                }
            }
            if (passProtect == 1) {
                continue;
            }
            //previous elements in the list
            for (long int i2 = i-1; i2 > -1; i2--) {
                if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                    pieceMoveValues[i]-=5;
                    break;
                }
            }
        }
        
        //checks for forks -> attacking pawns are registered
        int holdFork(0);
        int passFork(-1);
        for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
            //extracts the initial position of the piece -> does NOT include pawns
            int currentTemp = pieceMovesPawnsAttack[i]%100;
            int current = pieceMovesPawnsAttack[i] - currentTemp;
            if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
                passFork = i;
            }
            else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork > -1) {
                pieceMoveValues[passFork]-=20;
                pieceMoveValues[i]-=20;
                passFork = -2;
            }
            else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -2) {
                pieceMoveValues[i]-=20;
            }
            else {
                holdFork = current;
                passFork = -1;
                //repeat of the first IF statement -> mainly used for the first move
                if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
                    passFork = i;
                }
            }
        }
        
        //Top of decision structure to select a move
        long int nbPossPieceMoves = pieceMoveValues.size();
        
        float highestCaptureValue(10000);
        //list of best WHITE (2nd MOVE)
        //testing for highest move value
        for (int i(0); i < nbPossPieceMoves; i++) {
            if (pieceMoveValues[i] < highestCaptureValue) {
                highestCaptureValue = pieceMoveValues[i];
            }
        }
        return highestCaptureValue;
    }   //END WHITE
    
    //START BLACK
    if (playerKey == 66) {
        
        //looks at the next move. Skipped if moves ahead cap is reached.
        if (movesAhead < maxMoves) {
            movesAhead++;
            //goes through each move discovered and moves the piece -> used for looking move(s) ahead.
            for (int i(0); i < pieceMoves.size(); i++) {
            
            string AiMoveSTR = to_string(pieceMoves[i]);
            //extraction
            int tileSelectLi(int(AiMoveSTR[0])-48);
            int tileSelectCol(int(AiMoveSTR[1])-48);
            int moveLi(int(AiMoveSTR[2])-48);
            int moveCol(int(AiMoveSTR[3])-48);
            int pieceID = board[tileSelectLi][tileSelectCol];
            int pieceIDSave = board[moveLi][moveCol];
            int playerKeySave = ownership[moveLi][moveCol];
            
            //movement
            board[tileSelectLi][tileSelectCol] = int(32);
            board[moveLi][moveCol] = pieceID;
            ownership[tileSelectLi][tileSelectCol] = int(32);
            ownership[moveLi][moveCol] = playerKey;
            
            float pieceMoveValuesAddition = AiTestingSimulationMove(board, ownership, playerKey, movesAhead);
            pieceMoveValues[i]+=pieceMoveValuesAddition;
            
            //de-movement
            board[tileSelectLi][tileSelectCol] = pieceID;
            board[moveLi][moveCol] = pieceIDSave;
            ownership[tileSelectLi][tileSelectCol] = playerKey;
            ownership[moveLi][moveCol] = playerKeySave;
            }
        }
        
        //extra checks section (protections, forks) -> pins and forks sohuld be realized due to the looping system.
        //eleminates all non-pawn capture moves from the list -> used for protection/pin/fork tests
        vector<int> pieceMovesPawnsAttack;
        for (int i(0); i < pieceMoves.size(); i++) {
            
            string currentSTR = to_string(pieceMoves[i]);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            int moveCol(int(currentSTR[3])-48);
            int pieceID = board[tileSelectLi][tileSelectCol];
            
            if (pieceID == 80 && tileSelectCol == moveCol) {
            pieceMovesPawnsAttack.push_back(pieceMoves[i]%100);
            }
            else {
                pieceMovesPawnsAttack.push_back(pieceMoves[i]);
            }
        }
        
        //checks for protection.
        for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
            //Solution for protection on both end of the list -> prevents repeated protection points from stacking
            int passProtect = 0;
            int current = pieceMovesPawnsAttack[i]%100;
                
            //checks for pawns protects!
            //black PP
            string currentSTR = to_string(current);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            //pawns protect!
            if (board[tileSelectLi-1][tileSelectCol-1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
            else if (board[tileSelectLi-1][tileSelectCol+1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
            //next elements in the list
            for (long int i2 = i+1; i2 < pieceMovesPawnsAttack.size(); i2++) {
                if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                    pieceMoveValues[i]+=5;
                    passProtect = 1;
                    break;
                }
            }
            if (passProtect == 1) {
                continue;
            }
            //previous elements in the list
            for (long int i2 = i-1; i2 > -1; i2--) {
                if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                    pieceMoveValues[i]+=5;
                    break;
                }
            }
        }
        
        //checks for forks -> attacking pawns are registered
        int holdFork(0);
        int passFork(-1);
        for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
            //extracts the initial position of the piece -> does NOT include pawns
            int currentTemp = pieceMovesPawnsAttack[i]%100;
            int current = pieceMovesPawnsAttack[i] - currentTemp;
            if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
                passFork = i;
            }
            else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork > -1) {
                pieceMoveValues[passFork]+=20;
                pieceMoveValues[i]+=20;
                passFork = -2;
            }
            else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -2) {
                pieceMoveValues[i]+=20;
            }
            else {
                holdFork = current;
                passFork = -1;
                //repeat of the first IF statement -> mainly used for the first move
                if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
                    passFork = i;
                }
            }
        }
        
        //Top of decision structure to select a move
        long int nbPossPieceMoves = pieceMoveValues.size();
        
        float highestCaptureValue(-10000);
        //list of best BLACK (3rd MOVE)
        //testing for highest move value
        for (int i(0); i < nbPossPieceMoves; i++) {
            if (pieceMoveValues[i] > highestCaptureValue) {
                highestCaptureValue = pieceMoveValues[i];
            }
        }
        return highestCaptureValue;
    }   //END BLACK
    
    //IF THIS IS REACHED, THERE'S AN ISSUE
    return 0;
}


int AiPieceMove(int board[X][Y], int ownership[X][Y], int playerKey) {
    
    vector<int> pieceMoves;
    vector<float> pieceMoveValues;
        
    //determines the positions of all the pieces on the designated team
    vector<int> initialPiecePos;
    for (int li(1); li < 9; li++) {
    for (int col(1); col < 9; col++) {
        if (ownership[li][col] == char(playerKey)) {
            initialPiecePos.push_back((li*10 + col)*100);
        }
    }
    }
    
    //Determines ALL possible moves for the designated team.
    //Can be altered (change in playerKey) to determine ALL possible moves for the non-AI team.
    for (int possMoves = 0; possMoves < initialPiecePos.size(); possMoves+=1) {
    int masterLiCopy(int(to_string(initialPiecePos[possMoves])[0]-48));
    int masterColCopy(int(to_string(initialPiecePos[possMoves])[1]-48));
        
    //rook moves
    if (board[masterLiCopy][masterColCopy] == int(82)) {
        for (int directions(0); directions <4; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //left
                    while (colCopy > 0) {
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //up
                    while (liCopy > 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //right
                    while (colCopy < 9) {
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //down
                    while (liCopy < 9) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Rook move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
        }
    }
    
    //knight moves
    else if (board[masterLiCopy][masterColCopy] == int(78)) {
        for (int directions(0); directions <8; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            
            if (directions == 0) /*UUL*/ {
                liCopy-=2;
                colCopy-=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 1) /*UUR*/ {
                liCopy-=2;
                colCopy+=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 2) /*DDR*/ {
                liCopy+=2;
                colCopy+=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 3) /*DDL*/ {
                liCopy+=2;
                colCopy-=1;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 4) /*ULL*/ {
                liCopy-=1;
                colCopy-=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 5) /*URR*/ {
                liCopy-=1;
                colCopy+=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 6) /*DRR*/ {
                liCopy+=1;
                colCopy+=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }
                
            else if (directions == 7) /*DLL*/ {
                liCopy+=1;
                colCopy-=2;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                //Add start/end + capture value of Knight move
                if (liCopy>0 && liCopy<9 && colCopy>0 && colCopy<9 && STC == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                    pieceMoveValues.push_back(ETC);
                }
            }

            }
        }
    
    //bishop moves
    else if (board[masterLiCopy][masterColCopy] == int(66)) {
        for (int directions(0); directions <4; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //left-up
                    while (liCopy > 0 && colCopy > 0) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //left-down
                    while (liCopy < 9 && colCopy > 0) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //right-up
                    while (liCopy > 0 && colCopy < 9) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //right-down
                    while (liCopy < 9 && colCopy < 9) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Bishop move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
        }
    }
    
    //queen moves
    else if (board[masterLiCopy][masterColCopy] == int(81)) {
        for (int directions(0); directions <8; directions++) {
        int liCopy = masterLiCopy;
        int colCopy = masterColCopy;
            switch (directions) {
                case 0: //L
                    while (colCopy > 0) {
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 1: //U
                    while (liCopy > 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 2: //R
                    while (colCopy < 9) {
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 3: //D
                    while (liCopy < 9) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 4: //UL
                    while (liCopy > 0 && colCopy > 0) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 5: //DL
                    while (liCopy < 9 && colCopy > 0) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 6: //UR
                    while (liCopy > 0 && colCopy < 9) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
                    
                case 7: //DR
                    while (liCopy < 9 && colCopy < 9) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //Add start/end + capture value of Queen move
                        if (STC == 1 && valid == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                        else { break; }
                    }
                    break;
            }
            }
        }
    
    //king moves - castle-ing is NOT IMPLEMENTED INTO THE AI's move set.
    else if (board[masterLiCopy][masterColCopy] == int(75)) {
    for (int directions(0); directions <8; directions++) {
    int liCopy = masterLiCopy;
    int colCopy = masterColCopy;
        
        if (directions == 0) /*UL*/ {
            liCopy-=1;
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 1) /*U*/ {
            liCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 2) /*UR*/ {
            liCopy-=1;
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 3) /*R*/ {
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 4) /*DR*/ {
            liCopy+=1;
            colCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
         
        else if (directions == 5) /*D*/ {
            liCopy+=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 6) /*DL*/ {
            liCopy+=1;
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }
            
        else if (directions == 7) /*L*/ {
            colCopy-=1;
            int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
            int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
            //Add start/end + capture value of King move
            if (STC == 1 && valid == 1) {
                pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                pieceMoveValues.push_back(ETC);
            }
        }

        }
    }
    
    //pawn moves - en passent is NOT IMPLEMENTED INTO THE AI's move set.
    else if (board[masterLiCopy][masterColCopy] == int(80)) {
        //black pawns
        if (playerKey == int(66)) {
            //first move - two spaces move
            if (masterLiCopy == int(2)) {
                int liCopy = (masterLiCopy) + 2;
                int colCopy = masterColCopy;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                
                //Add start/end + capture value of Pawn move
                if (STC == 1 && valid == 1 && legalPawn == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    pieceMoveValues.push_back(0);
                }
            }
            
            //all other moves - 1 space move - capture moves
                for (int directions(0); directions <3; directions++) {
                    int liCopy = (masterLiCopy);
                    int colCopy = masterColCopy;
                    //1 space forward
                    if (directions == 0) {
                        liCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            pieceMoveValues.push_back(0);
                        }
                    }
                    //capturing to the left
                    else if (directions == 1) {
                        liCopy++;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                    //capturing to the right
                    else if (directions == 2) {
                        liCopy++;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
            }
        }
        
        //white pawns
        if (playerKey == int(87)) {
            //first move - two spaces move
            if (masterLiCopy == int(7)) {
                int liCopy = (masterLiCopy) - 2;
                int colCopy = masterColCopy;
                int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                
                //Add start/end + capture value of Pawn move
                if (STC == 1 && valid == 1 && legalPawn == 1) {
                    pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                    pieceMoveValues.push_back(0);
                }
            }
            
            //all other moves - 1 space move - capture moves
                for (int directions(0); directions <3; directions++) {
                    int liCopy = (masterLiCopy);
                    int colCopy = masterColCopy;
                    //1 space forward
                    if (directions == 0) {
                        liCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            pieceMoveValues.push_back(0);
                        }
                    }
                    //capturing to the left
                    else if (directions == 1) {
                        liCopy--;
                        colCopy--;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                    //capturing to the right
                    else if (directions == 2) {
                        liCopy--;
                        colCopy++;
                        int STC = sameTeamCapture(playerKey, liCopy, colCopy, ownership);
                        int valid = movementCheck(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership, STC, 1);
                        //the ghost pawns for en passent would be added where the two blank strings are in the 'movePawn' funtion call. The white ghost pawn is the first blank, the black ghost pawn is the second blank.
                        int legalPawn = movePawn(playerKey, masterLiCopy, masterColCopy, liCopy, colCopy, ownership,"  ","  ");
                        
                        //Add start/end + capture value of Pawn move
                        if (STC == 1 && valid == 1 && legalPawn == 1) {
                            pieceMoves.push_back((initialPiecePos[possMoves] + liCopy*10 + colCopy));
                            int ETC = enemyTeamCapture(playerKey, liCopy, colCopy, ownership, board);
                            pieceMoveValues.push_back(ETC);
                        }
                    }
                }
        }
    }
    }
    
    //goes through each move discovered and moves the piece -> used for looking move(s) ahead.
    for (int i(0); i < pieceMoves.size(); i++) {
        
        string AiMoveSTR = to_string(pieceMoves[i]);
        //extraction
        int tileSelectLi(int(AiMoveSTR[0])-48);
        int tileSelectCol(int(AiMoveSTR[1])-48);
        int moveLi(int(AiMoveSTR[2])-48);
        int moveCol(int(AiMoveSTR[3])-48);
        int pieceID = board[tileSelectLi][tileSelectCol];
        int pieceIDSave = board[moveLi][moveCol];
        int playerKeySave = ownership[moveLi][moveCol];
        
        //movement
        board[tileSelectLi][tileSelectCol] = int(32);
        board[moveLi][moveCol] = pieceID;
        ownership[tileSelectLi][tileSelectCol] = int(32);
        ownership[moveLi][moveCol] = playerKey;
        
        float pieceMoveValuesAddition = AiTestingSimulationMove(board, ownership, playerKey, 0);
        pieceMoveValues[i]+=pieceMoveValuesAddition;
        
        //de-movement
        board[tileSelectLi][tileSelectCol] = pieceID;
        board[moveLi][moveCol] = pieceIDSave;
        ownership[tileSelectLi][tileSelectCol] = playerKey;
        ownership[moveLi][moveCol] = playerKeySave;
    }
        
    //extra checks section (protections, forks) -> pins and forks sohuld be realized due to the looping system.
    //eleminates all non-pawn capture moves from the list -> used for protection/pin/fork tests
    //also adds points for promoting pawns -> all pawns are promoted into queens -> +6
    vector<int> pieceMovesPawnsAttack;
    for (int i(0); i < pieceMoves.size(); i++) {
        
        string currentSTR = to_string(pieceMoves[i]);
        int tileSelectLi(int(currentSTR[0])-48);
        int tileSelectCol(int(currentSTR[1])-48);
        int moveLi(int(currentSTR[2])-48);
        int moveCol(int(currentSTR[3])-48);
        int pieceID = board[tileSelectLi][tileSelectCol];
        
        //applies points for promoting pawns, sadly, the AI does not consider promotions when looking moves ahead. This can be implemented at a later date.
        if (pieceID == 80 && moveLi == 8) {
            pieceMoveValues[i]+=6;
        }
        
        if (pieceID == 80 && tileSelectCol == moveCol) {
            pieceMovesPawnsAttack.push_back(pieceMoves[i]%100);
        }
        else {
            pieceMovesPawnsAttack.push_back(pieceMoves[i]);
        }
    }
    
    //checks for protection. -> +5
    for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
        //Solution for protection on both end of the list -> prevents repeated protection points from stacking
        int passProtect = 0;
        int current = pieceMovesPawnsAttack[i]%100;
            
        //checks for pawns protects!
        //black PP
        if (playerKey == 66) {
            string currentSTR = to_string(current);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            //pawns protect!
            if (board[tileSelectLi-1][tileSelectCol-1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
            //This might cause an issue if a piece moves to the 8th column, it will try to check a non-existent portion of board. This does NOT crash the system, but it might cause an issue later. A simple IF statement to make the code bypass this check if a piece moves to the 8th column can be used if this does cause an issue in the future.
            else if (board[tileSelectLi-1][tileSelectCol+1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
        }
        //white PP
        else if (playerKey == 87) {
            string currentSTR = to_string(current);
            int tileSelectLi(int(currentSTR[0])-48);
            int tileSelectCol(int(currentSTR[1])-48);
            //pawns protect!
            if (board[tileSelectLi+1][tileSelectCol-1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
            else if (board[tileSelectLi+1][tileSelectCol+1] == 80) {
                pieceMoveValues[i]+=5;
                continue;
            }
        }
        //next elements in the list
        for (long int i2 = i+1; i2 < pieceMovesPawnsAttack.size(); i2++) {
            if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                pieceMoveValues[i]+=5;
                passProtect = 1;
                break;
            }
        }
        if (passProtect == 1) {
            continue;
        }
        //previous elements in the list
        for (long int i2 = i-1; i2 > -1; i2--) {
            if (current == pieceMovesPawnsAttack[i2]%100 && pieceMovesPawnsAttack[i2] > 100) {
                pieceMoveValues[i]+=5;
                break;
            }
        }
    }
    
    //checks for forks -> attacking pawns are registered -> +20
    int holdFork(0);
    int passFork(-1);
    for (int i(0); i < pieceMovesPawnsAttack.size(); i++) {
        //extracts the initial position of the piece -> does NOT include pawns
        int currentTemp = pieceMovesPawnsAttack[i]%100;
        int current = pieceMovesPawnsAttack[i] - currentTemp;
        if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
            passFork = i;
        }
        else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork > -1) {
            pieceMoveValues[passFork]+=20;
            pieceMoveValues[i]+=20;
            passFork = -2;
        }
        else if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -2) {
            pieceMoveValues[i]+=20;
        }
        else {
            holdFork = current;
            passFork = -1;
            //repeat of the first IF statement -> mainly used for the first move
            if (current == holdFork && pieceMoveValues[i] >= 9 && passFork == -1) {
                passFork = i;
            }
        }
    }
    
    //Top of decision structure to select a move
    long int nbPossPieceMoves = pieceMoveValues.size();
    
    float highestCaptureValue(-100000);
    vector<int> highestCaptureValueMove;
    
    //testing for highest move value
    
    //highestCaptureValue -> value of current 'best move'
    //highestCaptureValueLocation -> registers the 'best move' by the position of the move within the pieceMoveValues array. i.e. if the 'best move' is the 5th move in the pieceMoveValues array, it will register a 4 (arrays start at 0) so that that move can be found. This 'location value' can then be trnslated into the actual move by comparing the location value to the pieceMoves array.
    for (int i(0); i < nbPossPieceMoves; i++) {
        if (pieceMoveValues[i] > highestCaptureValue) {
            highestCaptureValueMove.clear();
            highestCaptureValueMove.push_back(pieceMoves[i]);
            highestCaptureValue = pieceMoveValues[i];
        }
        else if (pieceMoveValues[i] == highestCaptureValue) {
            highestCaptureValueMove.push_back(pieceMoves[i]);
        }
    }
    
    int nbMoves = rand() %highestCaptureValueMove.size();
    int AiMove = highestCaptureValueMove[nbMoves];
    
    return AiMove;
}

//AI IMPLEMENTATION BLOCK ABOVE

int main()
{
    //VERY IMPORTANT -> ensures that the AI selects a random move as opposed to preset-random move (makes the AI choose a different random move every game)
    srand(time(NULL));
    
    //Main menu
    string play;
    cout << "Do you want to play chess? Enter 'Yes' or 'No'." << endl;
    cin >> play;

//Descision panel (pt 1)
//Start game
//In order to simplify the selection panel, only the first letter of the question is considered. It is tranformed into a capital letter to ensure consistancy. If the answer starts with a 'Y', the game will start. Any other response will be interpreted as a negative response.
if (char(toupper(play[0])) == 'Y')
{
    clearScreen();
    //Define size and board name
    //'board' is the actual board that the players will ineract with.
    int board[X][Y];
    //'ownership' is a hidden board that contains the place of all the pieces. 'ownsership' is what is used to determine which team the pieces belong to. This is done in a seperate table in order to simplify many things.
    int ownership[X][Y];
    
    //board and ownership are defined as integer arrays. This means that all pieces will be identified with their ASCII table equivilants. Integers are used as a personal preference.
    
    //Initialize chessboard
    //Set top left corner of board to blank
    board[0][0] = int(32);
        
    //Initialize variables
    //Initialize the definitions for piece identification
    //Strings are used for their ability to store multiple pieces of data and for thes ease of accessing these pieces of data
    string baseComplex("RNBQKBNR");
    string basePawns("PPPPPPPP");
    
    //         !!!!!!!!!!!!!!!!     IMPORTANT     !!!!!!!!!!!!!!!!
    //Pieces do not need to be identified as alive, with the exception of the king. This is because when a piece is captured it's position is overwritten by the capturing piece's data, effectively removing it as a side effect.

    //Multiple FOR loops are used to fill in both arrays. FOR loops are used to prevent the code from becoming exceedingly long.
    //Filling the visible/playable array
    //Setting coordinate display (letter and number borders)
    for (int top(1); top < 9; top++)
    {
        board[0][top] = int(top + 64);
    }
    for (int side(1); side < 9; side++)
    {
        board[side][0] = int(side + 48);
    }
        
    //Creating black base (top end)
    for (int blComp(1); blComp < 9; blComp++)
    {
        board[1][blComp] = baseComplex[(blComp - 1)];
    }
    for (int blPawn(1); blPawn < 9; blPawn++)
    {
        board[2][blPawn] = basePawns[(blPawn - 1)];
    }
        
    //Creating white base (bottom end)
    for (int whPawn(1); whPawn < 9; whPawn++)
    {
        board[7][whPawn] = basePawns[(whPawn - 1)];
    }
    for (int whComp(1); whComp < 9; whComp++)
    {
        board[8][whComp] = baseComplex[(whComp - 1)];
    }
        
    //Filling center with blanks - important for structure
    for (int li(3); li < 7; li++)
    {
        for (int col(1); col < 9; col++)
        {
            board[li][col] = int(32);
        }
    }
         
    
    //Filling invisible array - used to determine ownership of pieces
    ownership[0][0] = int(32);
        
    //Adding coordinates to keep positions consitent between tables
    for (int top(1); top < 9; top++)
    {
        ownership[0][top] = int(top + 64);
    }
    for (int side(1); side < 9; side++)
    {
        ownership[side][0] = int(side + 48);
    }
        
    //Identfying positions of black pieces
    for (int blComp(1); blComp < 9; blComp++)
    {
        ownership[1][blComp] = int(66);
    }
    for (int blPawn(1); blPawn < 9; blPawn++)
    {
        ownership[2][blPawn] = int(66);
    }
        
    //Identifying positions of white pieces
    for (int whPawn(1); whPawn < 9; whPawn++)
    {
        ownership[7][whPawn] = int(87);
    }
    for (int whComp(1); whComp < 9; whComp++)
    {
        ownership[8][whComp] = int(87);
    }
        
    //Filling center with blanks - important for structure
    for (int li(3); li < 7; li++)
    {
        for (int col(1); col < 9; col++)
        {
            ownership[li][col] = int(32);
        }
    }
    
    
    //All gameplay variables are integers with a few exceptions. INT's are used since they are easier to manipulate then CHARs. In the few places that STRING's are used, the purpose of the string is to hold multiple pieces of data. In order to use this data, the string is separated into its components and the CHAR's that make up the strings are converted into INT's. Since ASCII table values are used, this makes manipulating and comparing them easier. Using INT's and converting with the ASCII table as opposed to CHAR's is a personal preference.
    
    //Initialize gameplay variables
    int turnRotation(0); //Determines whose turn it is. varies from 0-2.
    int playerKey(0); //Determines ownership, changes depending on turnRotation. Alternates between 87 (W) and 66 (B)
    int CLEAR(0); //Creates a gap to give the illusion of a clean interface. Only runs if no illegal move has occured.
    int FAULT(0); //CLEARS the interface and displays the  illegal move text. Only runs if an illegal move has occured
    string ghostPawnWh("  "); //Stores the position of a white pawn that has moved 2 spaces. Used for en passant.
    string ghostPawnBl("  "); //Stores the position of a black pawn that has moved 2 spaces. Used for en passent.
    string whKingPos("E8"); //Stores the position of the white king. Used to determine check.
    string blKingPos("E1"); //Stores the position of the black king. Used to determine check.
    string select; //Stores the position of the selected piece
    string move; //Stores the position of the tile that the selected piece is to be moved to.
    int kingCount(2); //Counts the number of kings still alive. Used to end the game.
    int pieceID(0); //Stores the piece ID (ascii value). Used to move pieces.
    int pieceID_promote(80); //Used for inCheckTest after a pawn has been promoted.
    int checkStatus(0); //Used to display inCheckTest result.
    
    //A WHILE loop is used to ensure that the game continues no matter how many turns are taken.
    //Begin gameplay - ask for player to identify a piece
    while (kingCount == 2)
    {
        //Displays the error message and clears the screen on the condition that an illegal action is taken
        if (FAULT == 1)
        {
            clearScreen();
                cout << "\n\nThe tile you selected either contains a piece belonging to the opponents team or does not contain a piece at all. Take another turn." << endl;
                turnRotation--;
                CLEAR = 0;
        }
        
        //Clears the screen on the condition that a legal action is taken
        if (CLEAR == 1) {
            clearScreen();
        }
        
        //Allows the inCheckTest to register the piece that a pawn promotes into.
        if (pieceID_promote != 80) {
            pieceID = pieceID_promote;
        }
        /*
        //Test to see if the opponent is now in check
        //This is the first example of a string being decomposed into its parts and converted into integers. The TOUPPER function is used to keep values consistent in case a lower case letter is inputted.
        //On the first iteration of the WHILE loop, this does nothing since the MOVE string has yet to be defined.
        int moveLi(move[1] - 48);
        int moveCol(toupper(move[0]) - 64);
        checkStatus = inCheckTest(playerKey, moveLi, moveCol, ownership, whKingPos, blKingPos, turnRotation, pieceID);
        */
        if (checkStatus == 1) {
            if (turnRotation == 0) {
                //This small math equation transforms 2 into 1 and 1 into 2. This is used to avoid using an IF statetement.
                cout << "player " << 1 << ", you are in check!" << endl;
            }
            else {
                cout << "player " << -turnRotation + 3 << ", you are in check!" << endl;
            }
        }
        
        pieceID_promote = 80;
        
        //Display board at every iteration
        //Checks the number of kings still alive
        kingCount = displayBoard(board,ownership);
        
        //End game successfully - 1 king has been defeted, the other king lives
        if (kingCount == 1)
        {
            if (turnRotation == 0) {
                cout << "Congratulations player " << 2 << "!" << endl;
            }
            else {
            cout << "Congratulations player " << -turnRotation + 3 << "!" << endl;
            }
            return 0;
        }
        
        //Resets the CLEAR and FAULT conditions
        CLEAR = 1;
        FAULT = 0;
        
        //Determine player turn, set playerKey value -> used to ensure that a player can only move their piece
        turnRotation++;
        //If turnRotation is set to 0, it should be reset to 2. This happens due to how the faulty selection system works.
        if (turnRotation == 0) {
            turnRotation = 2;
        }
        //A SWITCH is used to determine the playerKey. playerKey is a variable that is used to determine which team is the current active team. It used instead of turnRotation in order to make the ASCII values line up better.
        //A SWITCH is used instead of an IF because I wanted to try using a SWITCH. An IF can execute this same procedure with just as much ease.
        switch (turnRotation)
        {
            case 1: //White team
                //Player key is used to determine ownership of pieces
                playerKey = 87;
                //Reset the ghost pawn which is used for en passent -> This allws the opponent 1 turn to capture the pawn using en passent
                ghostPawnWh = "  ";
                cout << "It is player 1's turn! Your team is in capital letters!" << endl;
                break;
            case 2: //Black team
                //Notes are the same as above
                playerKey = 66;
                ghostPawnBl = "  ";
                turnRotation = 0;
                cout << "It is player 2's turn! Your team is in lower case letters!" << endl;
                break;
        }

        if (playerKey == 87) {
        //Player defines selection
    cout << "Enter the piece that you would like to move. Identify the tile using the identification keys on the side of the board. Always input the letter first. (Example: E7)." << endl;
    cin >> select;
        }
        else if (playerKey == 66) {
            cout << "AI is thinking. Please wait a moment." << endl;
        }
            
        
      //Split select string into readable variables
    int tileSelectLi(select[1] - 48);
    int tileSelectCol(toupper(select[0]) - 64);
        
        //Determines if the tile selected contains a piece belonging to the current player's team. Also verifies that the selected tile exists within the board -> there had been an issue where pieces were being identified in places where the board did not exist. 
        //STC stands for Same Team Capture
        
        //This ENTIRE block will be dedicated to player-only movement.
        if (playerKey == 87) {
            if (ownership[tileSelectLi][tileSelectCol] == playerKey && tileSelectLi < 9 && tileSelectCol < 9)
            {
                //Identifies the piece as a Rook
                if (board[tileSelectLi][tileSelectCol] == int(82))
                {
                    cout << "You have selected a Rook!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    //valid (movementCheck): verifies that there is no piece blocking the movement path
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, STC, 0);
                    //Rules for Rook movement
                    if ((tileSelectCol == moveCol || tileSelectLi == moveLi) && STC == 1 && valid == 1)
                    //Registers piece ID - used for movement
                    {
                        pieceID = 82;
                    }
                    //Condition for an illegal move
                    else {
                        CLEAR = illegalMoveText();
                        turnRotation--;
                        continue;
                    }
                }

                //Identifies the piece as a Knight
                else if (board[tileSelectLi][tileSelectCol] == int(78))
                {
                    cout << "You have selected a Knight!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    //Rules for Knight movement
                     if (((abs(tileSelectLi - moveLi) == 2 && abs(tileSelectCol - moveCol) == 1) || (abs(tileSelectLi - moveLi) == 1 && abs(tileSelectCol - moveCol) == 2)) && STC == 1)
                     //Registers the piece ID - used for movement
                     {
                         pieceID = 78;
                     }
                    //Condition for an illegal move
                   else {
                       CLEAR = illegalMoveText();
                       turnRotation--;
                       continue;
                   }
                }
                
                //Identifies the piece as a Bishop
                else if (board[tileSelectLi][tileSelectCol] == int(66))
                {
                    cout << "You have selected a Bishop!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    //valid (movementCheck): verifies that there is no piece blocking the movement path
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, STC, 0);
                    //Rules for Bishop movement
                    if ((abs(tileSelectLi - moveLi) == abs(tileSelectCol - moveCol)) && STC == 1 && valid == 1)
                    //Registers piece ID - used for movement
                    {
                        pieceID = 66;
                    }
                    //Condition for an illegal move
                    else {
                        CLEAR = illegalMoveText();
                        turnRotation--;
                        continue;
                    }
                }
                
                //Identifies the piece as a Queen
                else if (board[tileSelectLi][tileSelectCol] == int(81))
                {
                    cout << "You have selected the Queen!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    //valid (movementCheck): verifies that there is no piece blocking the movement path
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, STC, 0);
                    //Rules for Queen movement -> allowed both Rook and Bishop moves
                    if ((tileSelectCol == moveCol || tileSelectLi == moveLi || abs(tileSelectLi - moveLi) == abs(tileSelectCol - moveCol)) && STC == 1 && valid == 1)
                    //Registes the piece ID - used for movement
                    {
                        pieceID = 81;
                    }
                    //Condition for an illegal move
                    else {
                        CLEAR = illegalMoveText();
                        turnRotation--;
                        continue;
                    }
                }
                
                //Identifies the piece as a King
                else if (board[tileSelectLi][tileSelectCol] == int(75))
                {
                    cout << "You have selected the King!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    move[0] = toupper(move[0]);
                    int moveLi(move[1] - 48);
                    int moveCol(move[0] - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    //valid (movementCheck): verifies that there is no piece blocking the movement path
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, STC, 0);
                    //Test variables are defined to make the rules easier to read due to the repetitve nature of the rules.
                    int liTestK = abs(tileSelectLi - moveLi);
                    int colTestK = abs(tileSelectCol - moveCol);
                    //Rules for King movement
                    if (((colTestK == 0 &&  liTestK == 1) || (liTestK == 0 &&  colTestK == 1) || (liTestK == 1 && colTestK == 1)) && STC == 1 && valid == 1)
                    //Registers piece ID - used for movement
                    {
                        pieceID = 75;
                    }
                    
                    //Castle-ing a king has its own dedicated movement block due to it's unique nature in moving two pieces simultaneousely.

                    //Castle a king (white)
                    else if (playerKey == 87 && move == "G8" && select == "E8" && STC == 1 && valid == 1 && board[8][8] == int(82) && ownership[8][8] == playerKey)
                    {
                        //Move the King
                        board[tileSelectLi][tileSelectCol] = int(32);
                        board[moveLi][moveCol] = int(75);
                        ownership[tileSelectLi][tileSelectCol] = int(32);
                        ownership[moveLi][moveCol] = playerKey;
                        
                        //Move the Rook
                        board[8][8] = int(32);
                        board[8][6] = int(82);
                        ownership[8][8] = int(32);
                        ownership[8][6] = playerKey;
                    }
                    //Castle a king (white - queen side)
                    else if (playerKey == 87 && move == "C8" && select == "E8" && STC == 1 && valid == 1 && board[8][1] == int(82) && ownership[8][1] == playerKey)
                    {
                        //Move the King
                        board[tileSelectLi][tileSelectCol] = int(32);
                        board[moveLi][moveCol] = int(75);
                        ownership[tileSelectLi][tileSelectCol] = int(32);
                        ownership[moveLi][moveCol] = playerKey;
                        
                        //Move the Rook
                        board[8][1] = int(32);
                        board[8][4] = int(82);
                        ownership[8][1] = int(32);
                        ownership[8][4] = playerKey;
                    }
                    //Castle a king (black)
                    else if (playerKey == 66 && move == "G1" && select == "E1" && STC == 1 && valid == 1 && board[1][8] == int(82) && ownership[1][8] == playerKey)
                    {
                        //Move the King
                        board[tileSelectLi][tileSelectCol] = int(32);
                        board[moveLi][moveCol] = int(75);
                        ownership[tileSelectLi][tileSelectCol] = int(32);
                        ownership[moveLi][moveCol] = playerKey;
                        
                        //Move the Rook
                        board[1][8] = int(32);
                        board[1][6] = int(82);
                        ownership[1][8] = int(32);
                        ownership[1][6] = playerKey;
                    }
                    //Castle a king (black - queen side)
                    else if (playerKey == 66 && move == "C1" && select == "E1" && STC == 1 && valid == 1 && board[1][1] == int(82) && ownership[1][1] == playerKey)
                    {
                        //Move the King
                        board[tileSelectLi][tileSelectCol] = int(32);
                        board[moveLi][moveCol] = int(75);
                        ownership[tileSelectLi][tileSelectCol] = int(32);
                        ownership[moveLi][moveCol] = playerKey;
                        
                        //Move the Rook
                        board[1][1] = int(32);
                        board[1][4] = int(82);
                        ownership[1][1] = int(32);
                        ownership[1][4] = playerKey;
                    }
                    //Condition for an illegal move
                    else {
                        CLEAR = illegalMoveText();
                        turnRotation--;
                        continue;
                    }
                    
                    //Records the current position of both kings
                    //Used to determine if a team is in check
                    if (playerKey == 87) {
                        whKingPos = move;
                    }
                    else if (playerKey == 66) {
                        blKingPos = move;
                    }
                }
                
                //Identifies the piece as a Pawn
                else if (board[tileSelectLi][tileSelectCol] == int(80))
                {
                    //The pawn uses it's own dedicated movement block. This is because the pawn has movement exceptions (en passent). The piece ID is registered to prevent duplicate moves.
                    pieceID = 80;
                    cout << "You have selected a Pawn!" << endl;
                    //Asks the player for the end location of the selected piece
                    move = movePiece();
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    //Verifies the the move location is within the boundaries of the board
                    if (moveCol > 8 || moveLi > 8) {
                        FAULT = 1;
                        continue;
                    }
                    //STC (sameTeamCapture): Verifies that the piece is not capturing a piece on the same team
                    //valid (movementCheck): verifies that there is no piece blocking the movement path
                    int STC = sameTeamCapture(playerKey, moveLi, moveCol, ownership);
                    int valid = movementCheck(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, STC, 0);
                    
                    //Creates a ghost pawn for en passent
                    if (tileSelectCol == moveCol && (abs(moveLi - tileSelectLi) == 2)) {
                        //White
                        if (playerKey == 87)
                        {
                            ghostPawnWh = move;
                        }
                        //Black
                        else if (playerKey == 66)
                        {
                            ghostPawnBl = move;
                        }
                    }
                    //Due to the complacated nature of the Pawn movement structure, it is relocated to its own designated function.
                    //Rules for Pawn movement
                    int legalPawn = movePawn(playerKey, tileSelectLi, tileSelectCol, moveLi, moveCol, ownership, ghostPawnWh, ghostPawnBl);
                    
                    //legal 1 identifies a normal move, legal 2 identifies en passent
                    if ((legalPawn == 1 || legalPawn == 2) && STC == 1 && valid == 1)
                    
                    //movement block
                    {
                        board[tileSelectLi][tileSelectCol] = int(32);
                        board[moveLi][moveCol] = int(80);
                        ownership[tileSelectLi][tileSelectCol] = int(32);
                        ownership[moveLi][moveCol] = playerKey;
                        
                        if (legalPawn == 2)
                        //En passent
                        {
                            //For white
                            if (playerKey == 87)
                            {
                                int ghostLi(ghostPawnBl[1] - 48);
                                int ghostCol(ghostPawnBl[0] - 64);
                                board[ghostLi][ghostCol] = int(32);
                                ownership[ghostLi][ghostCol] = int(32);
                            }
                            //For black
                            else if (playerKey == 66)
                            {
                                int ghostLi(ghostPawnWh[1] - 48);
                                int ghostCol(ghostPawnWh[0] - 64);
                                board[ghostLi][ghostCol] = int(32);
                                ownership[ghostLi][ghostCol] = int(32);
                            }
                        }
                    }
                    
                    //Condition for an illegal move
                   else {
                        CLEAR = illegalMoveText();
                            turnRotation--;
                    }
                    
                    //Do not need to determine team for promotion since ownership is tied to the space and moves with the piece, ownership is not tied to the piece itself.
                    //Promotion
                        if ((moveLi == 1 || moveLi == 8) && STC == 1 && valid == 1) {
                            cout << "Your pawn has reached the other side of the board and is now eligible for a promotion! What piece would you like to promote your pawn into? Type the character in brackets to select the piece.\nRook (R)\nKnight (N)\nBishop (B)\nQueen (Q)" << endl;
                            //The promotion key is first identified as a string to prevent issues with a multiple character input.
                            string promoteTemp;
                            cin >> promoteTemp;
                            char promote = char(toupper(promoteTemp[0]));
                            board[moveLi][moveCol] = int(promote);
                            pieceID_promote = int(promote);
                        }
                }
                
                //movement block (not for pawns, see above)
                if (pieceID != int(80))
                {
                    int moveLi(move[1] - 48);
                    int moveCol(toupper(move[0]) - 64);
                    
                    board[tileSelectLi][tileSelectCol] = int(32);
                    board[moveLi][moveCol] = pieceID;
                    ownership[tileSelectLi][tileSelectCol] = int(32);
                    ownership[moveLi][moveCol] = playerKey;
                }
                
                //Test to see if the opponent is now in check
                //This is the first example of a string being decomposed into its parts and converted into integers. The TOUPPER function is used to keep values consistent in case a lower case letter is inputted.
                //On the first iteration of the WHILE loop, this does nothing since the MOVE string has yet to be defined.
                //Test to see if the opponent is now in check - HUMAN version
                int moveLi(move[1] - 48);
                int moveCol(toupper(move[0]) - 64);
                checkStatus = inCheckTest(playerKey, moveLi, moveCol, ownership, whKingPos, blKingPos, turnRotation, pieceID);
            }
            
            //In the case of an illegal selection - faulty selection system
           else
            {
                FAULT = 1;
            }
        }
        
        //This is the AI block
        else if (playerKey == 66) {
            int AiMoveINT = AiPieceMove(board, ownership, playerKey);
            string AiMoveSTR = to_string(AiMoveINT);
            int tileSelectLi(int(AiMoveSTR[0])-48);
            int tileSelectCol(int(AiMoveSTR[1])-48);
            int moveLi(int(AiMoveSTR[2])-48);
            int moveCol(int(AiMoveSTR[3])-48);
            pieceID = board[tileSelectLi][tileSelectCol];
            cout << "Last move:\n" << char(tileSelectCol + 64) << tileSelectLi << "-->" << char(moveCol + 64) << moveLi << "\nPiece selected: " << char(pieceID) << endl;
            
            board[tileSelectLi][tileSelectCol] = int(32);
            board[moveLi][moveCol] = pieceID;
            ownership[tileSelectLi][tileSelectCol] = int(32);
            ownership[moveLi][moveCol] = playerKey;
            
            //promotes to queen
            if (pieceID == 80 && moveLi == 8) {
                board[moveLi][moveCol] = 81;
                pieceID = 81;
            }
            
            //Test to see if the opponent is now in check - AI version
            checkStatus = inCheckTest(playerKey, moveLi, moveCol, ownership, whKingPos, blKingPos, turnRotation, pieceID);
        }
    }
    
    
}
    
//Descision panel (pt 2)
//Do not start game
else
{
    cout << "Have a nice day!" << endl;
}
    return 1;
}

