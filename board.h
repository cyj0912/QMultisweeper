#pragma once

#include <cstdlib>
#include <ctime>

enum ftype {
    normal,
    flag,
    complete
};

enum states {
    newgame,
    playing,
    gameover,
    finished
};

class Tile {
public:
    ftype flagType; //sentinel has this set to normal
    int numMines; //sentinel has this set to 99999
    bool isMine; //sentinel has this set to false

    Tile() {
        flagType = normal;
        numMines = 99999;
        isMine = false;
    }
};

class board {
public:
    Tile tiles[500][500];
    //  1 2 3 4 5
    //1 X X X X X
    //2 This is how the board is defined
    //3 X X X X X
    //4 X X X X X
    //5 X X X X X

    states gameState;
    int rows, cols, mines;
    int numSafezonesLeft;
    int pressedRow, pressedCol;

    board(int r, int c, int m) {
        this->rows = r;
        this->cols = c;
        this->mines = m;
        numSafezonesLeft = r * c - mines;
        //writeln("Left: ", numSafezonesLeft);
        this->gameState = newgame;
    }

    //Handle events------------------------------------------------------------
    void lmbPressed(int r, int c) {
        //pressedRow = r; pressedCol = c;
    }

    void lmbReleased(int r, int c) {
        //Check if game has started. If so, generate mines
        if(gameState == newgame) {
            generateMines(r, c);
            gameState = playing;
        }

        //TODO: Check if mouse is relesed on the same tile
        //If so, proceed:
        digTile(r, c);
    }

    void mmbPressed(int r, int c) {
    }

    void mmbReleased(int r, int c) {
        discoverTiles(r, c);
    }

    void rmbPressed(int r, int c) {
        if(tiles[r][c].flagType == normal)
            tiles[r][c].flagType = flag;
        else if(tiles[r][c].flagType == flag)
            tiles[r][c].flagType = normal;
    }

    void rmbReleased(int r, int c) {
    }

    //Internal functions-------------------------------------------------------
    void debugPrint() {
        /*write(rows);
        write('x');
        writeln(cols);
        for(int i = 1; i <= rows; i++) {
            for(int j = 1; j <= cols; j++) {
                write(tiles[i][j].numMines);
                write(tiles[i][j].isMine);
                write(' ');
            }
            writeln();
        }*/
    }

    int countFlags(int r, int c) {
        int ret = 0;
        for(int xshift = -1; xshift <= 1; xshift++)
            for(int yshift = -1; yshift <= 1; yshift++) {
                if(tiles[r+xshift][c+yshift].flagType == flag)
                    ret++;
            }
        return ret;
    }

    bool isSentinel(int r, int c) {
        return r == 0 || c == 0 || r == rows + 1 || c == cols + 1;
    }

    void digTile(int r, int c) {
        if(tiles[r][c].flagType == normal) {
            tiles[r][c].flagType = complete;
            if(!isSentinel(r, c)) {
                numSafezonesLeft--;
                //writeln("Left: ", numSafezonesLeft);
            }
            if(numSafezonesLeft == 0) {
                this->gameState = finished;
            }

            if(tiles[r][c].isMine) {
                this->gameState = gameover;
                return;
            }
            if(tiles[r][c].numMines == 0) {
                for(int xshift = -1; xshift <= 1; xshift++)
                    for(int yshift = -1; yshift <= 1; yshift++) {
                        digTile(r+xshift, c+yshift);
                    }
            }
        }
    }

    void discoverTiles(int r, int c) {
        //Only do discovery if the current tile is already cleared
        if(tiles[r][c].flagType == complete) {
            int numFlags = countFlags(r, c);
            if(numFlags == tiles[r][c].numMines) {
                for(int xshift = -1; xshift <= 1; xshift++)
                    for(int yshift = -1; yshift <= 1; yshift++) {
                        if(tiles[r+xshift][c+yshift].flagType == normal)
                            digTile(r+xshift, c+yshift);
                    }
            }
        }
    }

    void generateMines(int r, int c/*This specifies the tile to avoid*/) {
        srand(time(NULL));
        tiles[r][c].isMine = true;
        int numGenerated = 0;
        while(numGenerated < mines) {
            int targetr = rand() % rows + 1;
            int targetc = rand() % cols + 1;
            if(tiles[targetr][targetc].isMine)
                continue;
            tiles[targetr][targetc].isMine = true;
            numGenerated++;
        }
        tiles[r][c].isMine = false;
        for(int i = 1; i <= rows; i++)
            for(int j = 1; j <= cols; j++) {
                int mineCount = 0;
                for(int xshift = -1; xshift <= 1; xshift++)
                    for(int yshift = -1; yshift <= 1; yshift++) {
                        if(tiles[i+xshift][j+yshift].isMine)
                            mineCount++;
                    }
                tiles[i][j].numMines = mineCount;
            }
    }
};
