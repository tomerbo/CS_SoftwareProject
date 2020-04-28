#ifndef HISTORY_H_
#define HISTORY_H_

#include "game.h"

/** history structures **
 *	MoveList structure - Holds the changed cell coordinates (X,Y),
 *						 The old value and the new value,
 *						 The previous and next change the command did (stored in MoveList structure).
 * 	CommandMoves structure - Holds the information about the changes a command made.
 * 							 command: An enum Command to store which command made the change.
 * 							 moveListFirst: the first change the command made.
 * 							 moveListLast: the last change the command made.
 * 							 prev/next: the previous and next commands stored.
 * 	GameList structure - Holds the current state of the game and the first state of the game.
 * 						 curr: A pointer to the cell which is the current state.
 * 						 FirstNode: A pointer to the cell which holds the first state.
 */

/** history "inside" functions ** (function of self use)
 * createHistory - Create and allocate memory for an empty GameList.
 * destroyHistory - Free all memory used by the history(GameList).
 *
 * newCommandMoves - Adds a new command to history(after curr) and deletes all commands and moves before doing so.
 * addMoveToLastCommand - Adds a move(commands change) to the curr command (should be the last).
 * addBoardToLastCommand - Adds all changes to the board as separate moves.
 *
 * clearMoves - Clears the history of the game. (destroys and creates a new history)
 */

/** history "outside" functions ** (function for the commands)
 * undoMove - undo the last command changes to the board.
 * redoMove - redo the next command changes to the board.
 * resetMoves - reset the board to the first state (undo all possible commands).
 */

typedef struct MoveList{
	int X;
	int Y;
	int old;
	int new;
	struct MoveList* prev;
	struct MoveList* next;
} MoveList;

typedef struct CommandMoves{
	enum Command command;
	MoveList* moveListFirst;
	MoveList* moveListLast;
	struct CommandMoves* prev;
	struct CommandMoves* next;
} CommandMoves;

typedef struct GameList{
	CommandMoves* curr;
	CommandMoves* FirstNode;
} GameList;

/*** "inside" functions ***/
/* create/destroy */
GameList* createHistory();
void destroyHistory(Game* game);
/* add commands */
void newCommandMoves(Game* game, enum Command command);
void addMoveToLastCommand(Game* game, int X, int Y, int oldValue, int newValue);
void addBoardToLastCommand(Game* game, Board *OldBoard);
/* clear history */
void clearMoves(Game* game);


/*** "outside" functions ***/
int undoMove(Game* game, int enablePrinting);
int redoMove(Game* game);
void resetMoves(Game* game);

#endif /* HISTORY_H_ */
