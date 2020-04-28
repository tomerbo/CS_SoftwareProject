#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "solver.h"

#include "LP.h"
#include "ILP.h"


/*
 * Valid values checks
 */


/*
 * isLegalValue
 * Checks if value is a valid input in the board for x=i and y=j.
 */
int isLegalValue(Board *board, int i, int j, int value, int isJustFixed){
	int ii, jj;

	if(isJustFixed && !getCellIsFixed(board, i, j)){
		return 1;
	}

	if(value == 0){
		return 1;
	}
	/* Check row */
	for(ii=0; ii<getBoardSize(board); ++ii){
		if(isJustFixed && !getCellIsFixed(board, ii, j))
			continue;
		if(ii!=i && getCellNum(board, ii, j) == value){
			return 0;
		}
	}
	/* Check column */
	for(jj=0; jj<getBoardSize(board); ++jj){
		if(isJustFixed && !getCellIsFixed(board, i, jj))
			continue;
		if(jj!= j && getCellNum(board, i, jj) == value){
			return 0;
		}
	}

	/* Check block */
	for(ii = i-(i%getBlockWidth(board)); ii< i-(i%getBlockWidth(board))+getBlockWidth(board); ++ii){
		for(jj = j-(j%getBlockHeight(board)); jj< j-(j%getBlockHeight(board))+getBlockHeight(board); ++jj){
			if(isJustFixed && !getCellIsFixed(board, ii, jj))
				continue;
			if(!(ii==i && jj==j) && getCellNum(board, ii, jj) == value){
				return 0;
			}
		}
	}

	return 1;
}



/* return new supposed size of array without zeroes */
int zeroesToEnd(int* array, int length){
	int j, index, size;

	index = 0;
	for(j=0; j<length; ++j){
		if(array[j]!=0){
			array[index] = array[j];
			++index;
		}
	}

	size = index;

	for(j=index; j<length; ++j){
		array[j]=0;
	}

	return size;
}

/*
 * getValidDigits
 * returns an array with all valid digits for the cell
 *
 * changes length to the length of the returned array.
 */
int* getValidDigits(Board *board, int i, int j, int* length){
	int* validDigits = (int*)malloc(getBoardSize(board) * sizeof(int));
	int* newValidDigits;
	int ii, jj, newSize;
	int k;

	if(!validDigits){
		printf("[Error] getValidDigits has failed\n");
		exit(0);
	}

	for(k=0; k<getBoardSize(board); ++k){
		validDigits[k] = k+1;
	}

	/* row */
	for(ii=0; ii<getBoardSize(board); ++ii){
		if(getCellNum(board, ii, j) != 0){
			validDigits[getCellNum(board, ii, j)-1] = 0;
		}
	}
	/* column */
	for(jj=0; jj<getBoardSize(board); ++jj){
		if(getCellNum(board, i, jj) != 0){
			validDigits[getCellNum(board, i, jj)-1] = 0;
		}
	}

	/* block */
	for(ii = i-(i%getBlockWidth(board)); ii< i-(i%getBlockWidth(board))+getBlockWidth(board); ++ii){
		for(jj = j-(j%getBlockHeight(board)); jj< j-(j%getBlockHeight(board))+getBlockHeight(board); ++jj){
			if(getCellNum(board, ii, jj) != 0){
				validDigits[getCellNum(board, ii, jj)-1] = 0;
			}
		}
	}

	newSize = zeroesToEnd(validDigits, getBoardSize(board));
	if(newSize == 0){
		newValidDigits = NULL;
	}
	else{
		newValidDigits = (int*)malloc(newSize * sizeof(int));
		if(!newValidDigits){
			printf("[Error] getValidDigits has failed\n");
			exit(0);
		}

		for(k=0; k<newSize; ++k){
			newValidDigits[k] = validDigits[k];
		}
	}
	free(validDigits);

	*length = newSize;
	return newValidDigits;
}


/*
 * Erroneous check
 */

int isErroneousCell(Board *board, int x, int y, int isJustFixed){
	return !isLegalValue(board, x, y, getCellNum(board, x, y), isJustFixed);
}

int isErroneousBoard(Board *board, int isJustFixed){
	int x,y,N;
	N = getBoardSize(board);
	for(x=0;x<N;x++){
		for(y=0;y<N;y++){
			if(getCellNum(board,x,y) != 0){
				if(isErroneousCell(board,x,y,isJustFixed))
					return 1;
			}
		}
	}
	return 0;
}

int isBoardSolved(Board *board){
	int x,y,N;
	N = getBoardSize(board);
	for(x=0; x<N; ++x){
		for(y=0; y<N; ++y){
			if(getCellNum(board, x, y)==0 || isErroneousCell(board,x,y,0))
				return 0;
		}
	}
	return 1;
}



/*
 * Solving the board
 */
/***LP***/

/* Solve board using LP
 * Used by the guess command */
Board* solveBoardLP(Board *board, double D){
	Board* solBoard;
	double* sol = LPcalc(board, 1);
	if(sol == NULL)
		return NULL;
	solBoard = readBoardLP(sol, board, D);
	free(sol);
	return solBoard;
}

double* getHintWithScores(Board *board, int X, int Y){
	int k, N = getBoardSize(board);
	double* scoresXY;
	double* sol = LPcalc(board, 0);
	if(sol == NULL)
		return NULL;

	scoresXY = (double*)calloc(N, sizeof(double));
	if(!scoresXY){
		printf("[Error] getHintWithScores has failed\n");
		exit(0);
	}

	for(k=0; k<N; ++k){
		scoresXY[k] = sol[Y*N*N+X*N+k];
	}
	free(sol);

	return scoresXY;
}

/***LP***/

/* Solve board using ILP
 * Used by the generate command */
Board* solveBoardILP(Board *board){
	Board* solBoard;
	double* sol = ILPcalc(board);
	if(sol == NULL)
		return NULL;
	solBoard = readBoardILP(sol, board);
	free(sol);
	return solBoard;
}

int getHint(Board *board, int X, int Y){
	int i, N=getBoardSize(board);
	double* sol = ILPcalc(board);
	if(sol == NULL)
		return 0;

	for(i=0; i<N; ++i){
		if(sol[Y*N*N+X*N+i]==1)
			return i+1;
	}

	free(sol);
	return 0;
}

int isSolvableBoard(Board *board){
	double* sol = ILPcalc(board);
	if(sol == NULL)
		return 0;
	free(sol);
	return 1;
}




/*****************
 * num_solutions *
 *****************/


typedef struct cellInfo{
	int x,y;
	int num;
	int isFixed;
}Info;
typedef struct Node{
	Info *cell;
	struct Node *nextNode;
} Node;

typedef struct cellStack{
	Node *firstNode;
}Stack;

Stack *createStack(){
	Stack *stack;
	stack = (Stack*)malloc(sizeof(Stack));
	if(!stack){
		printf("[Error] createStack has failed\n");
		exit(0);
	}
	stack->firstNode = NULL;
	return stack;
}

void destroyStack(Stack *stack){
	if(stack->firstNode == NULL)
		free(stack);
	else
		printf("error in algo\n");
}

void insertNode(Stack *stack, Info *cell){
	Info *copyCell;
	Node *newNode;
	copyCell = (Info*)malloc(sizeof(Info));
	if(!copyCell){
		printf("[Error] insertNode has failed.\n");
		exit(0);
	}
	copyCell->x = cell->x;
	copyCell->y = cell->y;
	copyCell->num = cell->num;
	copyCell->isFixed = cell->isFixed;
	newNode = (Node*)malloc(sizeof(Node));
	if(!newNode){
		printf("[Error] insertNode has failed.\n");
		exit(0);
	}
	newNode->cell=copyCell;
	newNode->nextNode = stack->firstNode;
	stack->firstNode=newNode;
}

void removeNode(Stack *stack, Info *cell){
	Node *first;
	first = stack->firstNode;
	cell->x = first->cell->x;
	cell->y = first->cell->y;
	cell->num = first->cell->num;
	cell->isFixed = first->cell->isFixed;
	free(first->cell);
	stack->firstNode = first->nextNode;
	free(first);
}

void setInfo(Info *cell,int x,int y,int num,int isFixed){
	cell->x=x;
	cell->y = y;
	cell->num = num;
	cell->isFixed = isFixed;
}

/*
 * numOfSolutions
 * This functions is using exhaustive backtracking to find the number of solutions
 * to the given board of sudoku.
 *
 * starts from top left corner 0,0
 *
 * returns the number of solutions to the board
 */
unsigned long int numOfSolutions(Board *board){
	Stack *stack;
	Board *clone;
	Info *cell;
	int i,j;
	int X,Y;/*first empty cell cordinates*/
	int x,y;
	int k;
	int seenEmptyCell;
	int valid;
	unsigned long int numOfSol;

	clone = createEmptyBoard(board->blockWidth,board->blockHeight);
	deepCloneBoard(board, clone);
	seenEmptyCell = 0;
	numOfSol = 0;
	valid = 0;

	for(i=0;i<getBoardSize(clone);i++){
		for(j=0;j<getBoardSize(clone);j++){
			if(getCellNum(clone, j, i) == 0){
				X=j;
				Y=i;
				seenEmptyCell = 1;
				break;
			}
		}
		if(seenEmptyCell)
			break;
	}
	if(!seenEmptyCell)
		return 1;/*we estimate the board is not erroneous*/
	x = X;
	y = Y;
	/**************after we found first empty cell************/

	stack=createStack();
	cell = (Info*)calloc(1,sizeof(Info));
	if(!cell){
		printf("[Error] numOfSolutions has failed.\n");
		exit(0);
	}

	k=1;
	i=0;
	while(1){
		if(getCellIsFixed(board,x,y) || getCellNum(board,x,y) != 0){/*found cell with value*/
			if(x<getBoardSize(board)-1){
				x=x+1;
			}
			else if(y<getBoardSize(board)-1){
				x = 0;
				y = y + 1;
			}
			else{
				numOfSol++;
				removeNode(stack, cell);
				x=cell->x;
				y=cell->y;
				k=cell->num+1;
				setCellNum(clone,x,y,0);
			}
			continue;
		}
		for(; k<=getBoardSize(clone);k++){/*search for valid value for the current empty cell*/
			if(isLegalValue(clone, x, y, k, 0)){
				valid = 1;
				break;
			}
		}
		if(valid == 1){
			if(x == getBoardSize(clone)-1 && y == getBoardSize(clone)-1){
				numOfSol++;
				setCellNum(clone,x,y,0);
				k++;
				valid = 0;
				continue;
			}
			setInfo(cell,x,y,k,clone->board[x][y].isFixed);
			setCellNum(clone,x,y,k);
			insertNode(stack, cell);
			valid = 0;
			k=1;
			if(x<getBoardSize(board)-1){
				x=x+1;
			}
			else if(y<getBoardSize(board)-1){
				x = 0;
				y = y + 1;
			}
		}
		else{
			setCellNum(clone,x,y,0);
			if(stack->firstNode != NULL){
				removeNode(stack, cell);
				x=cell->x;
				y=cell->y;
				k=cell->num+1;
			}
			if(stack->firstNode==NULL && x == X && y == Y && k>getBoardSize(clone))
				break;
			valid = 0;
			continue;
		}
	}
	free(cell);
	destroyStack(stack);
	destroyBoard(clone);
	return numOfSol;
}





