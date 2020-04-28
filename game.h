#ifndef GAME_H_
#define GAME_H_

/** game structures **
 * Cell structure - Each Cell holds a number between 0 and BoardSize.
 * 					Where [1,BoardSize] are valid numbers in the board and 0 is a placeholder for an empty Cell.
 * 					Also, There is a "Boolean" integer to indicate whether the Cell is fixed or not.
 * Board structure - A Board Contains a 2D structure of Cells,
 * 					 The width and height of each block in board.
 * Game structure - A game which holds a pointer to the board.
 * 					An integer for the "Mark Errors" option.
 * 					The state of the game (Init/Solve/Edit).
 * 					And a GameList which is a structure that saves the moves in the current game.
 */

/** Board structure functions **
 * createEmptyBoard - Create and allocate memory for empty board of size blockWidth*blockHeight.
 * destroyBoard - Free all memory used by the board.
 *
 * deepCloneBoard - Given a board and an empty board that has the same size.
 * 					This function copies all values from the board to the empty ones (including fixed cells etc.)
 * numberOfEmptyCells - Returns the number of empty cells in the board.
 *
 * isBoardFull - Checks whether the board is full.
 *
 * getCellNum - Get the number in the Cell at <X,Y>.
 * setCellNum - Set the number in the Cell at <X,Y>.
 * getCellIsFixed - Get the isFixed parameter in the Cell at <X,Y>.
 * getBoardSize - Get the board size. (board size is the product of the width and height of a single block)
 * getBlockWidth - Get the board's block width.
 * getBlockHeight - Get the board's block height.
 */

/** Game structure functions **
 * createGame - Initialize an empty game.
 * destroyGame - Free all memory used by the game.
 *
 * printBoard - Prints the board (according to the game mode).
 */

/** Functions to edit the game **
 * generateXY - Given a game with a valid board for the command this
 * 				function generates a board (as described in the instructions)
 * autofillBoard - Given a game with a valid board for the command this
 * 				   function fill the board with "obvious" values (as described in the instructions)
 */

typedef struct Cell{
	int num; /* A number between 1 and 9, If 0 then the element is empty */
	int isFixed;
} Cell;

typedef struct Board{
	Cell **board;
	int blockWidth; /* denoted as n */
	int blockHeight; /* denoted as m */
} Board;

typedef struct Game{
	Board *board;
	int markErrors;
	enum {Init, Solve, Edit} state;
	struct GameList* moveHistory;
} Game;

/**/
Game* createGame();
void destroyGame(Game *game);
void printBoard(Game *game);
/**/
Board* createEmptyBoard(int blockWidth, int blockHeight);
void destroyBoard(Board *board);
void deepCloneBoard(Board *board, Board *newBoard);
int numberOfEmptyCells(Board *board);
int isBoardFull(Board *board);
int getCellNum(Board *board, int x, int y);
void setCellNum(Board *board, int x, int y, int num);
int getCellIsFixed(Board *board, int x, int y);
int getBoardSize(Board *board);
int getBlockWidth(Board *board);
int getBlockHeight(Board *board);
/**/
int generateXY(Game* game, int X, int Y);
void autofillBoard(Game *game);

#endif
