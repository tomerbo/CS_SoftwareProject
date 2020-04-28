#ifndef SOLVER_H_
#define SOLVER_H_

/** solver functions **
 * isLegalValue - Checks if the value is a valid input in the board for x=i and y=j.
 * getValidDigits - Returns an array of all valid digits for x=i,y=j.
 *
 * isErroneousCell - Check whether the cell is erroneous (if there is another cell with the same value
 * 					 in the same row/column/block)
 * isErroneousBoard - Check whether the board has any erroneous cells
 *
 * isBoardSolved - Check whether the board is a successfully solved board.
 *
 * solveBoardLP - Tries to solve the board using LP, if possible.
 * getHintWithScores - Returns the scores for possible inputs for a single cell.
 * solveBoardILP - Solves the board using ILP., if possible
 * getHint - Return a hint value for a single cell.
 * isSolvableBoard - Return whether the board is solvable or not.
 *
 * numOfSolutions - Returns the number of possible solutions for the given board.
 */

/* Valid values */
int isLegalValue(Board *board, int i, int j, int value, int isJustFixed);
int* getValidDigits(Board *board, int i, int j, int* length);

/* Erroneous check */
int isErroneousCell(Board *board, int x, int y, int isJustFixed);
int isErroneousBoard(Board *board, int isJustFixed);

int isBoardSolved(Board *board);

/* Solving the board */
/*LP*/
Board* solveBoardLP(Board *board, double D);
double* getHintWithScores(Board *board, int X, int Y);
/*ILP*/
Board* solveBoardILP(Board *board);
int getHint(Board *board, int X, int Y);
int isSolvableBoard(Board *board);

/* num_solutions */
unsigned long int numOfSolutions(Board *board);

#endif
