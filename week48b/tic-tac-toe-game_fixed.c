/*
 * tic-tac-toe-game_fixed.c
 * Copy of `tic-tac-toe-game.c` with fixes and safer input handling.
 * Fixes applied:
 * - printBoard formatting corrected
 * - checkWinner ignores empty cells
 * - playerMove validates input and bounds
 * - srand called once in main (not every computer move)
 * - safer replay prompt using `scanf(" %c", &response)` to skip newlines
 * - clearer win/lose messages
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove();
void computerMove();
char checkWinner();
void printWinner(char);

char board[3][3];
const char PLAYER = 'X';
const char COMPUTER = 'O';

int main()
{
    char winner = ' ';
    char response;

    /* Seed RNG once */
    srand((unsigned)time(NULL));

    do {
        winner = ' ';
        response = ' ';
        resetBoard();

        while (winner == ' ' && checkFreeSpaces() != 0) {
            printBoard();
            playerMove();
            winner = checkWinner();
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }

            computerMove();
            winner = checkWinner();
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }
        }

        printBoard();
        printWinner(winner);

        /* Prompt for replay. Leading space in format skips leftover whitespace/newline */
        printf("\nWould you like to play again? (Y/N): ");
        if (scanf(" %c", &response) != 1) {
            response = 'N';
        }
        response = toupper((unsigned char)response);

    } while (response == 'Y');

    printf("\nTHANKS FOR PLAYING!\n");
    return 0;
}

void resetBoard()
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

void printBoard()
{
    printf("\n %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
    printf("\n");
}

int checkFreeSpaces()
{
    int freeSpaces = 9;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != ' ') {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

void playerMove()
{
    int x = 0, y = 0;
    int rc;
    printf("The PLAYER: '%c'\nThe COMPUTER: '%c'\n", PLAYER, COMPUTER);
    while (1) {
        printf("Enter row #(1-3): ");
        rc = scanf(" %d", &x);
        if (rc != 1) { /* clear bad input */
            int ch; while ((ch = getchar()) != EOF && ch != '\n');
            printf("Invalid input. Please enter numbers between 1 and 3.\n");
            continue;
        }
        printf("Enter column #(1-3): ");
        rc = scanf(" %d", &y);
        if (rc != 1) {
            int ch; while ((ch = getchar()) != EOF && ch != '\n');
            printf("Invalid input. Please enter numbers between 1 and 3.\n");
            continue;
        }
        if (x < 1 || x > 3 || y < 1 || y > 3) {
            printf("Row/column must be between 1 and 3. Try again.\n");
            continue;
        }
        x--; y--;
        if (board[x][y] != ' ') {
            printf("Invalid move: spot already taken. Try again.\n");
            continue;
        }
        board[x][y] = PLAYER;
        break;
    }
}

void computerMove()
{
    int x, y;
    if (checkFreeSpaces() > 0) {
        do {
            x = rand() % 3;
            y = rand() % 3;
        } while (board[x][y] != ' ');
        board[x][y] = COMPUTER;
    }
}

char checkWinner()
{
    /* check rows */
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            return board[i][0];
        }
    }
    /* check columns */
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            return board[0][i];
        }
    }
    /* check diagonals */
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        return board[0][2];
    }
    return ' ';
}

void printWinner(char winner)
{
    if (winner == PLAYER) {
        printf("YOU WIN!\n");
    } else if (winner == COMPUTER) {
        printf("YOU LOSE!\n");
    } else {
        printf("IT'S A TIE!\n");
    }
}
