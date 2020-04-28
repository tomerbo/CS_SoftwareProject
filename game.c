#include <stdio.h>
#include <stdlib.h>

#include "main_aux.h"
#include "parser.h"
#include "game.h"
#include "solver.h"
#include "history.h"

/*/*************************************************
 * Functions for Creating and Destroying Game/Board*
 ***************************************************/

/*
 * newGame
 * Initialize an empty game.
 */
Game* createGame()
{
	Game* game = (Game*)calloc(1 , sizeof(Game));
	if(!game){
		printf("[Error] newGame has failed\n");
		exit(0);
	}
	game->markErrors = 1;
	game->state = Init;
	game->moveHistory = createHistory();
	return game;
}

/* Free all memory used by the game */
void destroyGame(Game *game){
	destroyBoard(game->board);
	destroyHistory(game);
	free(game);
}

/* Create an empty board by allocating memory */
Board* createEmptyBoard(int blockWidth, int blockHeight){
	int i, boardSize= blockWidth*blockHeight;
	Board *board = (Board*)calloc(1 , sizeof(Board));
	if(!board){
		printf("[Error] createEmptyBoard has failed\n");
		exit(0);
	}
	board->board = (Cell**)calloc(boardSize , sizeof(Cell*));
	if(!(board->board)){
		printf("[Error] createEmptyBoard has failed\n");
		exit(0);
	}
	board->blockHeight = blockHeight;
	board->blockWidth = blockWidth;

	for(i=0; i<boardSize; ++i){
		board->board[i] = (Cell*)calloc(boardSize , sizeof(Cell));
		if(!(board->board[i])){
			printf("Error: createEmptyBoard has failed\n");
			exit(0);
		}
	}
	return board;
}

/* Free all memory used by the board */
void destroyBoard(Board *board){
	int i;

	if(board==NULL)
		return;

	for(i=0; i<getBoardSize(board); ++i){
		free(board->board[i]);
	}
	free(board->board);
	free(board);
}



/*/*************************************************
 * 		Functions for the Board structure		   *
 ***************************************************/

/* Copy all cells parameters from board to newBoard  */
void deepCloneBoard(Board *board, Board *newBoard){
	int x, y;
	for(x=0; x<getBoardSize(board); ++x){
		for(y=0; y<getBoardSize(board); ++y){
			setCellNum(newBoard, x, y, getCellNum(board, x, y));
			newBoard->board[x][y].isFixed = board->board[x][y].isFixed;
		}
	}
}

/* Returns the number of empty cells in the board */
int numberOfEmptyCells(Board *board){
	int counter = 0;
	int x, y;
	for(x=0; x<getBoardSize(board); ++x){
		for(y=0; y<getBoardSize(board); ++y){
			if(getCellNum(board, x, y) == 0){
				counter += 1;
			}
		}
	}
	return counter;
}

/* Check whether the board is full */
int isBoardFull(Board *board){
	int x, y;
	for(x=0; x<getBoardSize(board); ++x){
		for(y=0; y<getBoardSize(board); ++y){
			if(getCellNum(board, x, y)==0){
				return 0;
			}
		}
	}
	return 1;
}

int getCellNum(Board *board, int x, int y){
	return board->board[x][y].num;
}
void setCellNum(Board *board, int x, int y, int num){
	board->board[x][y].num = num;
}
int getCellIsFixed(Board *board, int x, int y){
	return board->board[x][y].isFixed;
}
int getBoardSize(Board *board){
	return board->blockHeight * board->blockWidth;
}
int getBlockWidth(Board *board){
	return board->blockWidth;
}
int getBlockHeight(Board *board){
	return board->blockHeight;
}


/*/*************************************************
 * 		Functions for the Game structure		   *
 ***************************************************/

/* Prints the seperator row between blocks and at the border of the board */
void printSeparatorRow(int N, int m){
	int i;
	for(i=1; i<=(4*N)+m+1; ++i){
		printf("-");
	}
	printf("\n");
}

/* Prints the game board */
void printBoard(Game *game){
	int x,y;
	Board *board = game->board;
	int isMarkErrors = game->state==Edit ? 1 : game->markErrors;

	printSeparatorRow(getBoardSize(board), getBlockHeight(board));
	for(y=0; y<getBoardSize(board); ++y){
		for(x=0; x<getBoardSize(board); ++x){
			if(x%getBlockWidth(board) == 0){
				printf("|");
			}

			if(getCellNum(board, x, y) == 0){
				printf("    ");
			}
			else if(getCellIsFixed(board, x, y)){
				printf(" %2d.", getCellNum(board, x, y));
			}
			else if(isMarkErrors && isErroneousCell(board, x, y, 0)){
				printf(" %2d*", getCellNum(board, x, y));
			}
			else{
				printf(" %2d ", getCellNum(board, x, y));
			}
		}
		printf("|\n");
		if((y+1)%getBlockHeight(board) == 0){
			printSeparatorRow(getBoardSize(board), getBlockHeight(board));
		}
	}
}



/*/*************************************************
 * 			Functions to edit the game			   *
 ***************************************************/

/*
 * Generate
 */

/* Changes the board such that X random cells gets filled with valid digits
 * if one of the chosen cells has no valid digits then the function did not succeed
 * X should be equal or less to the amount of availabe empty cells */
int fillXEmptyCells(Board *board, int X){
	int *valid =NULL;
	int validArrLength;
	int k, x, y;

	for(k=1; k<=X; ++k){
		do {
			x=rand()%getBoardSize(board);
			y=rand()%getBoardSize(board);
		} while(getCellNum(board, x, y) != 0);
		valid = getValidDigits(board, x, y, &validArrLength);
		if(validArrLength == 0)
			return 0;
		setCellNum(board, x, y, valid[rand()%validArrLength]);
		free(valid);
	}
	return 1;
}

/* Clears random N filled cells
 * N should be equal or less then the amount of filled cells */
void clearNCells(Board *board, int N){
	int k, x, y;
	for(k=1; k<=N; ++k){
		do {
			x=rand()%getBoardSize(board);
			y=rand()%getBoardSize(board);
		} while(getCellNum(board, x, y) == 0);
		setCellNum(board, x, y, 0);
	}
}

/* Generates a puzzle from the existing board */
int generateXY(Game* game, int X, int Y){
	int i , N=getBoardSize(game->board);
	Board *randBoard = NULL;
	Board *SolvedBoard = NULL;

	for(i=1; i<=1000; ++i){
		randBoard = createEmptyBoard(getBlockWidth(game->board), getBlockHeight(game->board));
		deepCloneBoard(game->board, randBoard);
		if(fillXEmptyCells(randBoard, X)){
			SolvedBoard = solveBoardILP(randBoard);
			if(SolvedBoard != NULL){
				if(Y<N*N) /* if Y>=(BoardSize^2) do not clear any cell */
					clearNCells(SolvedBoard, (N*N)-Y);
				break;
			}
		}
		destroyBoard(randBoard);
	}
	if(i==1001){
		destroyBoard(SolvedBoard);
		return 0;
	}

	destroyBoard(randBoard);
	destroyBoard(game->board);
	game->board = SolvedBoard;
	return 1;
}


/*
 * Autofill
 */

/* Fills all "obvious" cells */
void autofillBoard(Game *game){
	int x, y;
	int *valid = NULL;
	int validArrLength;
	Board *Original = NULL;

	Original = createEmptyBoard(getBlockWidth(game->board), getBlockHeight(game->board));
	deepCloneBoard(game->board, Original);

	for(x=0; x<getBoardSize(Original); ++x){
		for(y=0; y<getBoardSize(Original); ++y){
			if(getCellNum(Original, x, y) == 0){
				valid = getValidDigits(Original, x, y, &validArrLength);
				if(validArrLength == 1){
					setCellNum(game->board, x, y, valid[0]);
					addMoveToLastCommand(game, x, y, 0, valid[0]);
				}
				if(validArrLength!=0)
					free(valid);
			}
		}
	}

	destroyBoard(Original);
}
