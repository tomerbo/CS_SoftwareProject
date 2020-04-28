#include <stdio.h>
#include <stdlib.h>

#include "command_types.h"
#include "game.h"
#include "files.h"
#include "history.h"
#include "solver.h"


int commandSolve(Game* game, char *path, int *isGameOver){
	Board* newBoard = NULL;
	if(!readBoard(&newBoard, path, 0))
		return 0;

	if(isErroneousBoard(newBoard, 1)){
		printf("[Error] The board has erroneous fixed cells and therefore cannot be loaded.\n");
		return 0;
	}

	destroyBoard(game->board);
	game->board = newBoard;
	game->state = Solve;

	clearMoves(game);

	printBoard(game);

	if(isBoardFull(game->board)){
		if(isBoardSolved(game->board))
			*isGameOver = 1;
		else
			printf("[Try Again] The solution you provided has errors. Try a different one.\n");
	}

	return 1;
}

void commandEdit(Game* game){
	destroyBoard(game->board);
	game->board = createEmptyBoard(3,3);
	game->state = Edit;

	clearMoves(game);

	printBoard(game);
}

int commandEdit_OP(Game* game, char *path){
	Board* newBoard = NULL;
	if(!readBoard(&newBoard, path, 1))
		return 0;

	destroyBoard(game->board);
	game->board = newBoard;
	game->state = Edit;

	clearMoves(game);

	printBoard(game);
	return 1;
}

void commandMarkErrors(Game* game, int X){
	game->markErrors = X;
}

void commandPrintBoard(Game* game){
	printBoard(game);
}

int commandSet(Game* game, int X, int Y, int Z, int *isGameOver){
	int oldValue;

	X = X-1;
	Y = Y-1;
	if(getCellIsFixed(game->board, X, Y)){ /* A fixed cell is possible only in solve mode */
		printf("[Error] The cell you are trying to change is fixed.\n");
		return 0;
	}
	oldValue = getCellNum(game->board, X, Y);
	setCellNum(game->board, X, Y, Z);

	newCommandMoves(game, Set);
	addMoveToLastCommand(game, X, Y, oldValue, Z);

	printBoard(game);

	if(game->state == Solve && isBoardFull(game->board)){
		if(isBoardSolved(game->board))
			*isGameOver = 1;
		else
			printf("[Try Again] The solution you provided has errors. Try a different one.\n");
	}

	return 1;
}

int commandValidate(Game* game){

	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous and therefore cannot be validated.\n");
		return 0;
	}

	if(isSolvableBoard(game->board))
		printf("The board is solvable.\n");
	else
		printf("The board is not solvable.\n");

	return 1;
}

int commandGuess(Game* game, double D, int *isGameOver){
	Board *solved, *Original;
	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous.\n");
		return 0;
	}

	Original = createEmptyBoard(getBlockWidth(game->board), getBlockHeight(game->board));
	deepCloneBoard(game->board, Original);

	solved = solveBoardLP(game->board, D);
	if(solved == NULL){
		printf("[Error] The board is not solvable with LP.\n");
		destroyBoard(Original);
		return 0;
	}

	destroyBoard(game->board);
	game->board = solved;

	newCommandMoves(game, Guess);
	addBoardToLastCommand(game, Original);
	destroyBoard(Original);

	printBoard(game);

	if(isBoardFull(game->board)){
		if(isBoardSolved(game->board))
			*isGameOver = 1;
		else
			printf("[Try Again] The solution you provided has errors. Try a different one.\n");
	}

	return 1;
}

int commandGenerate(Game* game, int X, int Y){
	Board *Original;
	if(numberOfEmptyCells(game->board) < X){
		printf("[Error] There are not enough empty cells. Please enter another amount of cells to fill and try again.\n");
		return 0;
	}

	Original = createEmptyBoard(getBlockWidth(game->board), getBlockHeight(game->board));
	deepCloneBoard(game->board, Original);

	if(!generateXY(game, X, Y)){
		printf("[Error] Puzzle generator could not generate a puzzle after 1000 iterations.\n");
		destroyBoard(Original);
		return 0;
	}

	newCommandMoves(game, Generate);
	addBoardToLastCommand(game, Original);
	destroyBoard(Original);

	printBoard(game);
	return 1;
}

int commandUndo(Game* game){
	if(!undoMove(game, 1)){
		printf("[Error] There are no commands to undo.\n");
		return 0;
	}
	printBoard(game);
	return 1;
}

int commandRedo(Game* game){
	if(!redoMove(game)){
		printf("[Error] There are no commands to redo.\n");
		return 0;
	}
	printBoard(game);
	return 1;
}

int commandSave(Game* game, char *path){
	int isEditMode = 0;
	if(game->state == Edit)
		isEditMode = 1;

	if(isEditMode && isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous and therefore cannot be saved in Edit mode.\n");
		return 0;
	}
	if(isEditMode && !isSolvableBoard(game->board)){
		printf("[Error] The board does not have a solution and therefore cannot be saved in Edit mode.\n");
		return 0;
	}
	saveBoard(game->board, path, isEditMode);

	return 1;
}

int commandHint(Game* game, int X, int Y){
	int hint;
	X=X-1;
	Y=Y-1;
	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous.\n");
		return 0;
	}
	if(getCellIsFixed(game->board,X,Y)){
		printf("[Error] The cell you are trying to get a hint for is fixed.\n");
		return 0;
	}
	if(getCellNum(game->board,X,Y) != 0){
		printf("[Error] The cell you are trying to get a hint for already has a value.\n");
		return 0;
	}

	hint = getHint(game->board, X, Y);

	if(hint == 0){
		printf("[Error] The board is not solvable.\n");
		return 0;
	}

	printf("Hint: You should enter %d to the cell (%d,%d).\n",hint, X+1, Y+1);

	return 1;
}

int commandGuessHint(Game* game, int X, int Y){
	int k;
	double *hint;
	X=X-1;
	Y=Y-1;
	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous.\n");
		return 0;
	}
	if(getCellIsFixed(game->board,X,Y)){
		printf("[Error] The cell you are trying to get a hint for is fixed.\n");
		return 0;
	}
	if(getCellNum(game->board,X,Y) != 0){
		printf("[Error] The cell you are trying to get a hint for already has a value.\n");
		return 0;
	}

	hint = getHintWithScores(game->board, X, Y);
	if(hint==NULL){
		printf("[Error] The board is not solvable.\n");
		return 0;
	}

	printf("Hints for (%d,%d) with scores:\n", X+1, Y+1);
	printf("Value\t:\tScore\n");
	for(k=0; k<getBoardSize(game->board); ++k){
		if(hint[k]>0)
			printf("%d\t:\t%f\n", k+1, hint[k]);
	}

	free(hint);
	return 1;
}

int commandNumSolutions(Game* game){
	unsigned long int numSol;
	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous.\n");
		return 0;
	}

	numSol = numOfSolutions(game->board);
	printf("The number of available solution for the board is: %lu .\n", numSol);

	return 1;
}

int commandAutofill(Game* game, int *isGameOver){

	if(isErroneousBoard(game->board, 0)){
		printf("[Error] The board is erroneous and therefore cannot be filled with \"obvious\" values.\n");
		return 0;
	}

	newCommandMoves(game, Autofill);

	autofillBoard(game);

	printBoard(game);

	if(isBoardFull(game->board)){
		if(isBoardSolved(game->board))
			*isGameOver = 1;
		else
			printf("[Try Again] The solution you provided has errors. Try a different one.\n");
	}
	return 1;
}

int commandReset(Game* game){
	resetMoves(game);
	printBoard(game);
	return 1;
}

