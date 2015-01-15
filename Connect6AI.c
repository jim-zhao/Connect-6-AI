// By: Jim Zhao
// Created on: November 18, 2013
// This is a artificial intelligence that plays the Connect 6 board game, and was entered in a tournament
// Program is on one file because tournament specifications required it to be that way

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int n;
int squares;
int result = 1; // Result of the game. Currently set to draw
char board [19][19];
int axes[4][19][19]; // 1: +slopediag, 2: -slopediag, 3: horizontal, 4: vertical
bool play[19][19];
bool Prev[19][19];
int Longest = 0;
int count = 0;
int move = 1;

char color; // player that the computer is
char opp; // player human is

void Check() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            if (play[i][j])
                printf("%d %d\n", i, j);
    }
}

void Play(int row, int col) {
    for (int m = row - 1; m < row + 2; m++) {
        for (int n = col - 1; n < col + 2; n++) {
            play[m][n] = true;
        }
    }
}

char Switch(char p) {
    if (p == 'W')
        return 'B';
    else
        return 'W';
}

int min(int a, int b) {
    if (a < b)
        return a;
    else
        return b;
}

void Initialize() // Initializing the entire board to empty
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                axes[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[i][j] = 'U';
        }
    }
}

void printboard() // Outputting the entire board
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

void printAxes() // Printing the 3D array that shows how pieces are connected. For debugging purposes.
{
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                printf("%d  ", axes[k][i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("------\n");
}

bool CheckBounds(int row, int col) // Checking to make sure that the program keeps within the array (board) boundaries
{
    if (row > -1 && row < n && col > -1 && col < n) {
        return true; // Continue if it is in bounds
    }
    return false; // Exit if it is not in bounds
}

void OneAxis(int row, int col, char p, int ri, int ci, int axesuse, int d) // Checking blocks along a diretion
{
    row += ri;
    col += ci;
    while (CheckBounds(row, col) && board[row][col] == p) {
        axes[axesuse][row][col] += d;
        row += ri;
        col += ci;
    }
}

void Block(int row, int col, char p, int direction) // Checking for blocks in all directions
{
    p = Switch(p);
    OneAxis(row, col, p, 1, 1, 0, direction); // Down right
    OneAxis(row, col, p, -1, -1, 0, direction); // Up left
    OneAxis(row, col, p, 1, -1, 1, direction); // Down left
    OneAxis(row, col, p, -1, 1, 1, direction); // Up right
    OneAxis(row, col, p, 0, 1, 2, direction); // right
    OneAxis(row, col, p, 0, -1, 2, direction); // left
    OneAxis(row, col, p, 1, 0, 3, direction); // down
    OneAxis(row, col, p, -1, 0, 3, direction); // up
}

void LongestAxis(int row, int col, int rp, int cp, int a) // Finding how the stones are connected and how many are connected in a row
{
    int cur = 1, blocks = 2;
    char mem = board[row][col];
    char sp = Switch(mem);
    int CRow = row + rp;
    int CCol = col + cp;
    while (CheckBounds(CRow, CCol) && board[CRow][CCol] == mem) {
        cur++;
        CRow += rp;
        CCol += cp;
    } // If the value here is the same as the first point, continue. Continue until this does not exist
    if (CheckBounds(CRow, CCol) && board[CRow][CCol] != sp) // This means that the system was blocked
    {
        blocks--;
    }
    CRow = row - rp;
    CCol = col - cp;
    while (CheckBounds(CRow, CCol) && board[CRow][CCol] == mem) {
        cur++;
        CRow -= rp;
        CCol -= cp;
    }
    if (CheckBounds(CRow, CCol) && board[CRow][CCol] != sp) // This means that the system was blocked
    {
        blocks--;
    }

    for (int i = 0; i < cur; i++) {
        CRow += rp;
        CCol += cp;
        axes[a][CRow][CCol] = 10 * cur + blocks;
    }
    if (cur > Longest) // If there is a new record for pieces in line, max will store that new record
    {
        Longest = cur;
    }
}

void InLongest(int row, int col) // Finding the longest series of stones after adding a piece
{
    LongestAxis(row, col, 1, 1, 0);
    LongestAxis(row, col, 1, -1, 1);
    LongestAxis(row, col, 0, 1, 2);
    LongestAxis(row, col, 1, 0, 3);

}

int Evaluate(char p) // The evaluation method of the program. The is where the program 'thinks'. Like a human, it will consider various factors in a situation and then generate a score to determine how good the situation is for itself. This method was iteratively conceptualized, tested, and improved to optimize its efficiency. 
{
    int factor = 1;
    int score = 0;
    int add = 0;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum = 0;
            if (board[i][j] == color) {
                factor = 1;
            } else if (board[i][j] == opp) {
                factor = -1;
            } else {
                continue;
            }
            for (int k = 0; k < 4; k++) // Axes to look at
            {
                add = 0;
                if (axes[k][i][j] > 55) {
                    return factor * 1000000;
                } else if (axes[k][i][j] > 45) {
                    add = factor * 3000;
                } else if (axes[k][i][j] > 35) {
                    add = factor * 1000;
                } else if (axes[k][i][j] > 25) {
                    add = factor * 400;
                } else if (axes[k][i][j] > 15) {
                    add = factor * 200;
                } else {
                    add = factor * 40;
                }
                if (axes[k][i][j] % 10 == 2) // Two blocks
                {
                    add = 0;
                } else if (axes[k][i][j] % 10 == 1) // One block
                {
                    add /= 2;
                }
                sum += add;
            }
            if (p != board[i][j]) {
                sum *= 21;
                sum /= 20;
            }
            sum += factor * min(j, n - j);
            sum += factor * min(i, n - i);
            score += sum;

        }

    }
    return score;
}

void Update(int row, int col, char p) {
    InLongest(row, col);
    Block(row, col, p, 1);

}

int MoveSearch(int row, int col, char p, int depth) // The recursion, implements the minimax algorithm
{
    int CLongest = Longest;
    int Eval;

    int Before[5][19][19];
    bool PastPlay[19][19];

    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Before[k][i][j] = axes[k][i][j];
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Before[4][i][j] = board[i][j];
            PastPlay[i][j] = play[i][j];
        }
    }
    board[row][col] = p;
    Update(row, col, p);
    Play(row, col);
    Eval = Evaluate(p);

    if (depth == 2 || Longest > 5) // The term condition
    {
        board[row][col] = 'U';
        // Reversal
        for (int k = 0; k < 4; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    axes[k][i][j] = Before[k][i][j];
                }
            }
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                board[i][j] = Before[4][i][j];
                play[i][j] = PastPlay[i][j];
            }
        }
        Longest = CLongest;
        //
        return Eval;
    } else {
        p = Switch(p);
        if (p == opp) // Human move
        {
            int min = 9999999;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (play[i][j] && board[i][j] == 'U') {
                        Eval = MoveSearch(i, j, p, depth + 1);
                        if (Eval < min) {
                            min = Eval;
                        }
                    }
                }
            }
            //
            board[row][col] = 'U';
            for (int k = 0; k < 4; k++) {
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        axes[k][i][j] = Before[k][i][j];
                    }
                }
            }
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    board[i][j] = Before[4][i][j];
                    play[i][j] = PastPlay[i][j];
                }
            }
            Longest = CLongest;
            //
            return min;
        } else // Computer move
        {
            int max = -9999999;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (play[i][j] && board[i][j] == 'U') {
                        Eval = MoveSearch(i, j, p, depth + 1);
                        if (Eval > max) {
                            max = Eval;
                        }
                    }
                }
            }
            //
            board[row][col] = 'U';
            for (int k = 0; k < 4; k++) {
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < n; j++) {
                        axes[k][i][j] = Before[k][i][j];
                    }
                }
            }
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    board[i][j] = Before[4][i][j];
                    play[i][j] = PastPlay[i][j];
                }
            }
            Longest = CLongest;
            //
            return max;
        }
    }
}

void ComputerMove(char p) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Prev[i][j] = play[i][j];
        }
    }

    int max = -9999999;
    int s = max;
    int row = n / 2, col = n / 2;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {

            if (Prev[i][j] && board[i][j] == 'U') {
                s = MoveSearch(i, j, p, 1);
                if (s > max) {
                    max = s;
                    row = i;
                    col = j;
                }
            }
        }
    }
    board[row][col] = p;
    Update(row, col, p);
    Play(row, col);
    printf("Computer lays a stone at ROW %d COL %d.\n", row, col);
}

void HumanMove(char p) {
    /* // For playing against itself, this was part of the testing process
     color = opp;
     opp = Switch(opp);
     ComputerMove(p);
     color = opp;
     opp = Switch(opp);
     */

    int row, col;
    printf("Lay down a stone (ROW COL): ");
    scanf("%d%d", &row, &col);
    if (board[row][col] == 'U') {
        board[row][col] = p;
        Update(row, col, p);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (axes[1][i][j]) {
                    for (int m = i - 1; m < i + 2; m++) {
                        for (int n = j - 1; n < j + 2; n++) {
                            play[m][n] = true;

                        }
                    }
                }
            }
        }
    } else {
        printf("That square is occupied.\n");
        HumanMove(p); // Call to ask again
    }

}

bool End(char p) // Determine if the game is over, and what way
{
    bool draw = true;
    if (Longest > 5) {
        if (p == 'W')
            result = 0;
        else
            result = 2;
        return true;
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == 'U')
                    draw = false;
            }
        }
        return draw;
    }

}

void order() {
    if (color == 'B') {
        while (!End('W')) {
            if (move == 1) {
                board[n / 2][n / 2] = color;
                Update(n / 2, n / 2, color);
                printf("Computer lays a stone at ROW %d COL %d.\n", n / 2, n / 2);
            } else
                ComputerMove('B');
            move++;
            printboard();
            if (End('B')) {
                break;
            }
            HumanMove('W');
            printboard();
            move++;
        }
    } else if (color == 'W') {
        while (!End('W')) {
            HumanMove('B');
            printboard();
            move++;
            if (End('B')) {
                break;
            }
            ComputerMove('W');
            printboard();
            move++;
        }
    }
}

int main() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            play[i][j] = false;
    }
    printf("Enter board dimensions (n): ");
    scanf("%d", &n);
    // Functions
    Initialize();
    printboard();
    printf("Computer playing B or W?: ");
    scanf(" %c", &color);
    opp = Switch(color);
    order(); // This is where the moves take place

    if (result == 0) {
        printf("White player wins.\n");
    } else if (result == 1) {
        printf("Draw!\n");
    } else {
        printf("Black player wins.\n");
    }
    return 0; // Exiting the program
}
